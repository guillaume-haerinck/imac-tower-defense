#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <string>
#include <vector>

#include "entity-factories/tile-factory.hpp"
#include "graph.hpp"

class Map {
public:
	Map(entt::DefaultRegistry& registry, const char* itdFilePath, glm::vec2& viewTranslation, float& viewScale);

	int Map::getTile(unsigned int x, unsigned int y) const;
	glm::vec2 windowToGrid(float x, float y);
	glm::vec2 projToGrid(float x, float y);
	glm::vec2 gridToProj(unsigned int x, unsigned int y);
	glm::vec2 getNodePosition(int nodeIndex);

	unsigned int getGridWidth() const;
	unsigned int getGridHeight() const;

	std::vector<glm::vec2> trajectory(); //From start to end
	std::vector<glm::vec2> trajectory(int node1); //From node1 to end
	std::vector<glm::vec2> trajectory(int node1, int node2); //From node1 to node2


	Graph m_graph;
	Graph m_pathfindingGraph;

	void drawGraph();
	void drawGrid();

private:
	glm::vec3 getPixelColorFromImage(unsigned char* image, int imageWidth, int x, int y);
	glm::vec3 getColorFromString(std::string line);
	float getNumberFromString(std::string line);

	//For the graph construction :
	bool isPath(unsigned char* image, int imageWidth, int imageHeight, int x, int y);
	bool isStraightLine(unsigned char* image, int imageWidth, int imageHeight, int x, int y);
	void lookForNodes(unsigned char* image, int imageWidth, int imageHeight, int parentNodeIndex, int x, int y, int xDir, int yDir, int travelLength);
	//Pathfinding graph :
	void constructPathfindingGraph();

private:
	// .ITD file data
	glm::vec3 m_pathColor;
	glm::vec3 m_nodeColor;
	glm::vec3 m_startColor;
	glm::vec3 m_endColor;
	glm::vec3 m_constructColor;
	std::string m_mapPath;
	float m_energy;

	// Map details
	std::vector<unsigned int> m_map;
	unsigned int m_gridWidth;
	unsigned int m_gridHeight;

	// Aggregation
	entt::DefaultRegistry& m_registry;
	TileFactory m_tileFactory;

	// External info
	const glm::vec2& m_viewTranslation;
	const float& m_viewScale;
};
