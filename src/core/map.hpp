#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "tile.hpp"

class Map {
public:
    Map(const char* itdFilePath);
    ~Map();

    Tile* Map::getTile(unsigned int x, unsigned int y);
    glm::vec2 windowToGrid(float x, float y);
    glm::vec2 gridToWindow(unsigned int x, unsigned int y);
    void render(); // Mettre ça ici, vraiment ?
	// TODO use flyweight pattern to say the tile type
	// http://gameprogrammingpatterns.com/flyweight.html

private:
    glm::vec3 getPixelColorFromImage(unsigned char* image, int imageWidth, int x, int y);
    glm::vec3 getColorFromString(std::string line);
    float getNumberFromString(std::string line);

private:
    glm::vec3 m_pathColor;
    glm::vec3 m_nodeColor;
    glm::vec3 m_startColor;
    glm::vec3 m_endColor;
    glm::vec3 m_constructColor;
    std::string m_mapPath;
    float m_energy;
    std::vector<Tile*> m_map;
    unsigned int m_gridWidth;
    unsigned int m_gridHeight;

    // Tile types
    Tile constructible;
    Tile path;
    Tile locked;
    Tile pipe;
    Tile entry;
    Tile exit;
};
