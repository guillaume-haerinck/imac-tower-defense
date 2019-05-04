#include "map.hpp"

#include <fstream>
#include <sstream>
#include <stb_image/stb_image.h>
#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"
#include "logger/gl-log-handler.hpp"

#include "core/constants.hpp"
#include "core/maths.hpp"

// TODO use a public function to change the current map, and read the itd file
// TODO rename MAP to Level for clear separation with world map

Map::Map(entt::DefaultRegistry& registry, const char* itdFilePath, glm::vec2& viewTranslation, float& viewScale)
	: m_registry(registry), m_tileFactory(registry), m_mapPath("res/maps/"), m_viewTranslation(viewTranslation), m_viewScale(viewScale)
{
	/* ---------------------------- Read ITD file ------------------------- */
	std::ifstream file(itdFilePath);
	if (file.is_open()) {
		/* // TODO check first line to see if valid itd
		if (.find("@ITD") != std::string::npos)
		{
		spdlog::error("[Error] File provided does not have @ITD");
		break;
		}
		*/

		// TODO increment count valid, related to itd version, if a field is missing say it

		std::string line;
		while (std::getline(file, line)) {
			if (line.find("#") != std::string::npos) { continue; } // Skip comments
			else if (line.find("carte") != std::string::npos) { m_mapPath += line.substr(6, line.size()); }
			else if (line.find("energie") != std::string::npos) { m_energy = getNumberFromString(line); }
			else if (line.find("chemin") != std::string::npos) { m_pathColor = getColorFromString(line); }
			else if (line.find("noeud") != std::string::npos) { m_nodeColor = getColorFromString(line); }
			else if (line.find("construct") != std::string::npos) { m_constructColor = getColorFromString(line); }
			else if (line.find("in") != std::string::npos) { m_startColor = getColorFromString(line); }
			else if (line.find("out") != std::string::npos) { m_endColor = getColorFromString(line); }
		}
		file.close();
	}
	else {
		spdlog::critical("[ITD] Unable to open file");
	}

	/* --------------------------- Read Png file ------------------------- */
	int imgWidth, imgHeight, imgChannels;
	// Because 0,0 is bottom left in OpenGL
	stbi_set_flip_vertically_on_load(true);
	unsigned char *image = stbi_load(m_mapPath.c_str(), &imgWidth, &imgHeight, &imgChannels, STBI_rgb);
	if (nullptr == image) {
		spdlog::critical("Echec du chargement de l'image de carte '{}'", m_mapPath);
		debug_break();
	}
	stbi_set_flip_vertically_on_load(false);

	m_gridWidth = imgWidth;
	m_gridHeight = imgHeight;
	m_map.resize(m_gridWidth * m_gridHeight);

	glm::vec3 color = glm::vec3(0);
	for (int y = 0; y < imgHeight; y++) {
		for (int x = 0; x < imgWidth; x++) {
			color = getPixelColorFromImage(image, imgWidth, x, y);
			glm::vec2 position = gridToProj(x, y);
			unsigned int entityId = 0;

			if (color == m_pathColor) {
				entityId = m_tileFactory.createPath(position);
			}
			else if (color == m_nodeColor) {
				entityId = m_tileFactory.createPath(position);
			}
			else if (color == m_startColor) {
				entityId = m_tileFactory.createSpawn(position);
			}
			else if (color == m_endColor) {
				entityId = m_tileFactory.createArrival(position);
				//Construct graph
				//We start from the endpoint because there will always be only one on the map
				m_graph.addEndNode(m_graph.addNode(x, y));
				if (isPath(image, imgWidth, imgHeight, x + 1, y)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x + 1, y, 1, 0, 1);
				}
				if (isPath(image, imgWidth, imgHeight, x - 1, y)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x - 1, y, -1, 0, 1);
				}
				if (isPath(image, imgWidth, imgHeight, x, y + 1)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x, y + 1, 0, 1, 1);
				}
				if (isPath(image, imgWidth, imgHeight, x, y - 1)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x, y - 1, 0, -1, 1);
				}
				//Construct pathfinfing graph
				constructPathfindingGraph();
				//
			}
			else if (color == m_constructColor) {
				entityId = m_tileFactory.createConstructible(position);
			}
			else {
				entityId = m_tileFactory.createLocked(position);
			}
			m_map.at(y * m_gridWidth + x) = entityId;
		}
	}
	stbi_image_free(image);
}

/* ----------------------- PUBLIC GETTERS & SETTERS ----------------- */

int Map::getTile(unsigned int x, unsigned int y) const {
	try {
		return m_map.at(y * m_gridWidth + x);
	}
	catch (const std::out_of_range e) {
		spdlog::warn("Tile out of map range");
		return -1;
	}
}

unsigned int Map::getGridWidth() const { return m_gridWidth; }
unsigned int Map::getGridHeight() const { return m_gridHeight; }

glm::vec2 Map::windowToGrid(float x, float y) {
	float projX = imac::rangeMapping(x, 0, WIN_WIDTH, 0, PROJ_WIDTH);
	float projY = imac::rangeMapping(y, 0, WIN_HEIGHT, 0, PROJ_HEIGHT);
	return projToGrid(projX, projY);
}

glm::vec2 Map::projToGrid(float x, float y) {
	// TODO Fixme
	//spdlog::info("scale is {}", m_viewScale);
	//spdlog::info("translation is {} {}", m_viewTranslation.x, m_viewTranslation.y);

	float posX = WIN_RATIO * x - m_viewTranslation.x;
	//posX = imac::rangeMapping(posX, 0, m_gridWidth * TILE_SIZE * m_viewScale, 0, m_gridWidth * TILE_SIZE);
	float posY = y - m_viewTranslation.y;
	//posY = imac::rangeMapping(posY, 0, m_gridHeight * TILE_SIZE * m_viewScale, 0, m_gridHeight * TILE_SIZE);

	//spdlog::info("posY is {}", posY);

	unsigned int tileX = imac::rangeMapping(posX, 0, m_gridWidth * TILE_SIZE, 0, m_gridWidth);
	unsigned int tileY = imac::rangeMapping(posY, 0, m_gridHeight * TILE_SIZE, 0, m_gridHeight);
	return glm::vec2(tileX, tileY);
}

glm::vec2 Map::gridToProj(unsigned int x, unsigned int y) {
	float posX = imac::rangeMapping(x, 0, m_gridWidth, 0, m_gridWidth * TILE_SIZE);
	float posY = imac::rangeMapping(y, 0, m_gridHeight, 0, m_gridHeight * TILE_SIZE);
	return glm::vec2(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2);
}

glm::vec2 Map::getNodePosition(int nodeIndex) {
	return gridToProj(m_graph.getNode(nodeIndex).x, m_graph.getNode(nodeIndex).y);
}

/* ----------------------- PRIVATE GETTERS & SETTERS ---------------- */

glm::vec3 Map::getPixelColorFromImage(unsigned char* image, int imageWidth, int x, int y) {
	glm::vec3 pixel;
	pixel.r = image[3 * (y * imageWidth + x) + 0];
	pixel.g = image[3 * (y * imageWidth + x) + 1];
	pixel.b = image[3 * (y * imageWidth + x) + 2];
	return pixel;
}

float Map::getNumberFromString(std::string line) {
	std::string temp;
	float data;
	std::stringstream ss(line);
	while (!ss.eof())
	{
		ss >> temp;
		if (std::stringstream(temp) >> data)
		{
			return data;
		}
	}

	spdlog::warn("[ITD] no valid data at : {}", line);
	return 0.0f;
}

glm::vec3 Map::getColorFromString(std::string line) {
	std::string temp;
	float data;
	std::stringstream ss(line);
	std::vector<float> tempData;
	while (!ss.eof())
	{
		ss >> temp;
		if (std::stringstream(temp) >> data)
		{
			tempData.push_back(data);
		}
	}

	// Save temp data
	glm::vec3 color = glm::vec3(0);
	if (tempData.size() == 3) {
		color.r = tempData.data()[0];
		color.g = tempData.data()[1];
		color.b = tempData.data()[2];
	}
	else {
		spdlog::warn("[ITD] invalid colors at : {}", line);
	}

	return color;
}

/* ----------------------- AUXILARY FUNCTIONS FOR GRAPH CONSTRUCTION ----------------- */

bool Map::isPath(unsigned char* image, int imageWidth, int imageHeight, int x, int y) {
	glm::vec3 color = getPixelColorFromImage(image, imageWidth, x, y);
	return x >= 0
		&& x < imageWidth
		&& y >= 0
		&& y < imageHeight
		&& (color == m_pathColor || color == m_startColor || color == m_endColor);
}

bool Map::isStraightLine(unsigned char* image, int imageWidth, int imageHeight, int x, int y) {
	return isPath(image, imageWidth, imageHeight, x - 1, y)
		&& isPath(image, imageWidth, imageHeight, x + 1, y)
		&& !isPath(image, imageWidth, imageHeight, x, y - 1)
		&& !isPath(image, imageWidth, imageHeight, x, y + 1)
		||
		!isPath(image, imageWidth, imageHeight, x - 1, y)
		&& !isPath(image, imageWidth, imageHeight, x + 1, y)
		&& isPath(image, imageWidth, imageHeight, x, y - 1)
		&& isPath(image, imageWidth, imageHeight, x, y + 1);
}

void Map::lookForNodes(unsigned char* image, int imageWidth, int imageHeight, int parentNodeIndex, int x, int y, int xDir, int yDir, int travelLength) {
	glm::vec3 color = getPixelColorFromImage(image, imageWidth, x, y);
	if (isStraightLine(image, imageWidth, imageHeight, x, y) && color != m_startColor && color != m_endColor) {
		lookForNodes(image, imageWidth, imageHeight, parentNodeIndex, x + xDir, y + yDir, xDir, yDir, travelLength + 1);
	}
	else {
		int nodeIndex = m_graph.nodeIndex(x, y);

		if (nodeIndex == -1) { //Node hasn't been added yet

			int newNodeIndex = m_graph.addNode(x, y);
			m_graph.addNeighbouring(parentNodeIndex, newNodeIndex, travelLength);
			if (color == m_startColor) {
				m_graph.addStartNode(newNodeIndex);
			}
			//check forward
			if (isPath(image, imageWidth, imageHeight, x + xDir, y + yDir)) {
				lookForNodes(image, imageWidth, imageHeight, newNodeIndex, x + xDir, y + yDir, xDir, yDir, 1);
			}
			//check left
			int newXdir = -yDir;
			int newYdir = xDir;
			if (isPath(image, imageWidth, imageHeight, x + newXdir, y + newYdir)) {
				lookForNodes(image, imageWidth, imageHeight, newNodeIndex, x + newXdir, y + newYdir, newXdir, newYdir, 1);
			}
			//check right
			newXdir = yDir;
			newYdir = -xDir;
			if (isPath(image, imageWidth, imageHeight, x + newXdir, y + newYdir)) {
				lookForNodes(image, imageWidth, imageHeight, newNodeIndex, x + newXdir, y + newYdir, newXdir, newYdir, 1);
			}
		}
		else { //Node already added
			m_graph.addNeighbouring(parentNodeIndex, nodeIndex, travelLength, true);
		}
	}
}

/* ----------------------- PATHFINDING GRAPH ----------------- */

void Map::constructPathfindingGraph() {
	//Using Dijkstra algorithm

	//Find all distances to endNode
	//Initialize
	int endNode = m_graph.getEndNode();
	int N = m_graph.getNodesCount();
	float* dists = (float*)malloc(N * sizeof(float));
	int* prevNode = (int*)malloc(N * sizeof(int));
	bool* isDone = (bool*)malloc(N * sizeof(bool));
	for (int k = 0; k < N; ++k) {
		dists[k] = std::numeric_limits<float>::infinity();
		prevNode[k] = -1;
		isDone[k] = false;
	}
	dists[endNode] = 0;

	//Loop:
	int currentNode = 0; //arbitrary initialization, just has to be != -1
	while (currentNode != -1) {
		//Look for node with min dist to endNode
		currentNode = -1;
		for (int k = 0; k < N; ++k) {
			if (!isDone[k] && (currentNode == -1 || dists[k] < dists[currentNode])) {
				currentNode = k;
			}
		}
		if (currentNode != -1) {
			isDone[currentNode] = true;
			//
			//Visit current node's neighbours
			std::vector<graphEdge>* neighbours = m_graph.getNeighbours(currentNode);
			for( int i = 0 ; i < neighbours->size() ; ++i ) {
				float distTest = dists[currentNode] + neighbours->at(i).dist;
				int neighbour = neighbours->at(i).neighbourIndex;
				if (distTest < dists[neighbour]) {
					dists[neighbour] = distTest;
					prevNode[neighbour] = currentNode;
				}
			}
		}
	}
	//Construct new graph with stochastic weights
	//Nodes
	for (int n = 0; n < N; ++n) {
		m_pathfindingGraph.addNode(m_graph.getNode(n).x, m_graph.getNode(n).y);
	}
	//Edges
	for (int n = 0; n < N; ++n) {
		float dist = dists[n];
		float neighboursDistSum = 0;
		std::vector<graphEdge>* neighbours = m_graph.getNeighbours(n);
		for (int i = 0; i < neighbours->size(); ++i) {
			int neighbour = neighbours->at(i).neighbourIndex;
			float d = dists[neighbour];
			if (m_graph.distEstimator(neighbour) < m_graph.distEstimator(n) || d < dist) {
				neighboursDistSum += d;
			}
		}
		for (int i = 0; i < neighbours->size(); ++i) {
			int neighbour = neighbours->at(i).neighbourIndex;
			float d = dists[neighbour];
			if (m_graph.distEstimator(neighbour) < m_graph.distEstimator(n) || d < dist) {
				if (neighboursDistSum == 0 || d == neighboursDistSum) {
					m_pathfindingGraph.addNeighbourTo(n, neighbour, 1);
				}
				else {
					m_pathfindingGraph.addNeighbourTo(n, neighbour, 1 - d / neighboursDistSum);
				}
			}
		}
	}
	//I'm freeeeee !
	free(dists);
	free(prevNode);
	free(isDone);
}

/* ----------------------- DRAW ----------------- */

void Map::drawGraph() {
	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setColor(255, 0, 100, 1);
	GLCall(glPointSize(13));

	// Draw vertices
	for (int k = 0; k < m_graph.getNodesCount(); ++k) {
		float x = m_graph.getNode(k).x*100. / m_gridWidth + 50. / m_gridWidth;
		float y = m_graph.getNode(k).y*100. / m_gridHeight + 50. / m_gridHeight;
		debugDraw.point(x, y);
	}

	// Draw edges
	for (int k = 0; k < m_graph.getNodesCount(); ++k) {
		float x1 = m_graph.getNode(k).x*100.0f / m_gridHeight + 50. / m_gridHeight;
		float y1 = m_graph.getNode(k).y*100.0f / m_gridHeight + 50. / m_gridHeight;
		std::vector<graphEdge>* neighbours = m_graph.getNeighbours(k);

		for( int i = 0 ; i < neighbours->size() ; ++i ){
			float x2 = m_graph.getNode(neighbours->at(i).neighbourIndex).x*100.0f / m_gridHeight + 50. / m_gridHeight;
			float y2 = m_graph.getNode(neighbours->at(i).neighbourIndex).y*100.0f / m_gridHeight + 50. / m_gridHeight;
			debugDraw.line(x1, y1, x2, y2);
		}
	}
}

void Map::drawGrid() {
	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setColor(1, 1, 1, 1);

	for (int x = 0; x <= PROJ_WIDTH_RAT; x += TILE_SIZE) {
		debugDraw.line(x, 0, x, PROJ_HEIGHT);
	}

	for (int y = 0; y <= PROJ_HEIGHT; y += TILE_SIZE) {
		debugDraw.line(0, y, PROJ_WIDTH_RAT, y);
	}
}

/* ----------------------- PATHFINDING ----------------- */

/*std::vector<glm::vec2> Map::trajectory() {
	return trajectory(m_graph.getStartNodes().at(0), m_graph.getEndNode());
}

std::vector<glm::vec2> Map::trajectory(int node1) {
	return trajectory(node1, m_graph.getEndNode());
}

std::vector<glm::vec2> Map::trajectory(int node1, int node2) {
	std::vector<int> graphTraj = m_graph.trajectory(node1, node2);
	std::vector<glm::vec2> traj(graphTraj.size());
	for (int n = 0; n < traj.size(); ++n) {
		traj.at(n) = gridToProj(m_graph.getNode(graphTraj.at(n)).x, m_graph.getNode(graphTraj.at(n)).y);
	}
	return traj;
}*/