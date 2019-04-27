#include "map.hpp"

#include <fstream>
#include <sstream>
#include <stb_image/stb_image.h>
#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

#include "core/constants.hpp"
#include "core/maths.hpp"

Map::Map(entt::DefaultRegistry& registry, const char* itdFilePath)
:	m_registry(registry), m_tileFactory(registry), m_mapPath("res/maps/")
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
            else if (line.find("carte") != std::string::npos) {     m_mapPath += line.substr(6, line.size()); }
            else if (line.find("energie") != std::string::npos) {   m_energy = getNumberFromString(line); }
            else if (line.find("chemin") != std::string::npos) {    m_pathColor = getColorFromString(line); }
            else if (line.find("noeud") != std::string::npos) {     m_nodeColor = getColorFromString(line); }
            else if (line.find("construct") != std::string::npos) { m_constructColor = getColorFromString(line); }
            else if (line.find("in") != std::string::npos) {        m_startColor = getColorFromString(line); }
            else if (line.find("out") != std::string::npos) {       m_endColor = getColorFromString(line); }
        }
        file.close();
    } else {
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
			} else if (color == m_nodeColor) {
				entityId = m_tileFactory.createPath(position);
			} else if (color == m_startColor) {
				entityId = m_tileFactory.createSpawn(position);
			} else if (color == m_endColor) {
				entityId = m_tileFactory.createArrival(position);
				//Constructs adjency list
				//We start from the endpoint because there will always be only one on the map
				m_graph.addNode(x, y);
				if (isPath(image, imgWidth, imgHeight, x + 1, y)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x+1, y, 1, 0, 1);
				}
				if (isPath(image, imgWidth, imgHeight, x - 1, y)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x-1, y, -1, 0, 1);
				}
				if (isPath(image, imgWidth, imgHeight, x, y + 1)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x, y+1, 0, 1, 1);
				}
				if (isPath(image, imgWidth, imgHeight, x, y - 1)) {
					lookForNodes(image, imgWidth, imgHeight, 0, x, y-1, 0, -1, 1);
				}
				//
			} else if (color == m_constructColor) {
				entityId = m_tileFactory.createConstructible(position);
			} else {
				entityId = m_tileFactory.createLocked(position);
			}
			m_map.at(y * m_gridWidth + x) = entityId;
        }
    }
    stbi_image_free(image);
}

Map::~Map() {
}

/* ----------------------- PUBLIC GETTERS & SETTERS ----------------- */

unsigned int Map::getTile(unsigned int x, unsigned int y) {
    return m_map.at(y * m_gridWidth + x);
}

unsigned int Map::getGridWidth()  { return m_gridWidth; }
unsigned int Map::getGridHeight() { return m_gridHeight; }

glm::vec2 Map::windowToGrid(float x, float y) {
	float projX = imac::rangeMapping(x, 0, WIN_WIDTH, 0, PROJ_WIDTH);
	float projY = imac::rangeMapping(y, 0, WIN_HEIGHT, 0, PROJ_HEIGHT);
	return projToGrid(projX, projY);
}

glm::vec2 Map::projToGrid(float x, float y) {
	unsigned int tileX = imac::rangeMapping(x * WIN_RATIO, 0, m_gridWidth * TILE_SIZE, 0, m_gridWidth);
	unsigned int tileY = imac::rangeMapping(y, 0, m_gridHeight * TILE_SIZE, 0, m_gridHeight);
	return glm::vec2(tileX, tileY);
}

glm::vec2 Map::gridToProj(unsigned int x, unsigned int y) {
	float posX = imac::rangeMapping(x, 0, m_gridWidth, 0, m_gridWidth * TILE_SIZE);
	float posY = imac::rangeMapping(y, 0, m_gridHeight, 0, m_gridHeight * TILE_SIZE);
	return glm::vec2(posX + TILE_SIZE / 2, posY + TILE_SIZE / 2);
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
    } else {
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

bool Map::isStraightLine(unsigned char* image, int imageWidth, int imageHeight , int x, int y) {
	//bool isHorizontalLine = isPath(image, imageWidth, imageHeight, x - 1, y) && isPath(image, imageWidth, imageHeight, x + 1, y);
	//bool isVerticalLine = isPath(image, imageWidth, imageHeight, x , y - 1) && isPath(image, imageWidth, imageHeight, x , y + 1);
	//return isHorizontalLine != isVerticalLine;
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
	if (isStraightLine(image, imageWidth, imageHeight, x, y)) {
		lookForNodes(image, imageWidth, imageHeight, parentNodeIndex, x + xDir, y + yDir, xDir, yDir, travelLength + 1);
	}
	else {
		int nodeIndex = m_graph.nodeIndex(x, y);

		if (nodeIndex == -1) { //New node

			int newNodeIndex = m_graph.addNode(x, y);
			m_graph.addNeighbouring(parentNodeIndex, newNodeIndex);

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
			m_graph.addNeighbouring(parentNodeIndex, nodeIndex,true);
		}
	}
}