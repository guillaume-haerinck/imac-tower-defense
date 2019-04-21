#pragma once

#include <glm/glm.hpp>
#include <string>

class Map {
public:
    Map(const char* itdFilePath);
    ~Map();

private:
    glm::vec3 getPixelColors(unsigned char* image, int imageWidth, int x, int y);
    glm::vec3 getDataColors(std::string line);
    float getData(std::string line);

private:
    glm::vec3 m_pathColor;
    glm::vec3 m_nodeColor;
    glm::vec3 m_startColor;
    glm::vec3 m_endColor;
    glm::vec3 m_constructColor;
    std::string m_mapPath;
    float m_energy;
};
