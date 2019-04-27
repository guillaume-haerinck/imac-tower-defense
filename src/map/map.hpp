#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "entity-factories/tile-factory.hpp"
#include "graph.hpp"

class Map {
public:
    Map(entt::DefaultRegistry& registry, const char* itdFilePath);
    ~Map();

    unsigned int Map::getTile(unsigned int x, unsigned int y);
    glm::vec2 windowToGrid(float x, float y);
	glm::vec2 projToGrid(float x, float y);
    glm::vec2 gridToProj(unsigned int x, unsigned int y);

	unsigned int getGridWidth();
	unsigned int getGridHeight();

private:
    glm::vec3 getPixelColorFromImage(unsigned char* image, int imageWidth, int x, int y);
    glm::vec3 getColorFromString(std::string line);
    float getNumberFromString(std::string line);
	inline long rangeMapping(long x, long in_min, long in_max, long out_min, long out_max);

	//For the graph construction :
	bool isPath(unsigned char* image, int imageWidth, int imageHeight, int x, int y);
	bool isStraightLine(unsigned char* image, int imageWidth, int imageHeight, int x, int y);
	void lookForNodes(unsigned char* image, int imageWidth, int imageHeight, int parentNodeIndex,  int x, int y, int xDir, int yDir , int travelLength);

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
	Graph m_graph;

	// Aggregation
	entt::DefaultRegistry& m_registry;
	TileFactory m_tileFactory;
};
