#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "entity-factories/tile-factory.hpp"

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

    // Tile entities (flyweight pattern: only one entity by tile type)
	unsigned int m_constructibleId;
	unsigned int m_pathId;
	unsigned int m_lockedId;
	unsigned int m_pipeId;
	unsigned int m_entryId;
	unsigned int m_exitId;
};
