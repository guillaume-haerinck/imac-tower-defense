#include "map.hpp"

#include <fstream>
#include <sstream>
#include <stb_image/stb_image.h>
#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

Map::Map(const char* itdFilePath) {
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
            else if (line.find("carte") != std::string::npos) {     m_mapPath = line.substr(6, line.size()); }
            else if (line.find("energie") != std::string::npos) {   m_energy = getData(line); }
            else if (line.find("chemin") != std::string::npos) {    m_pathColor = getDataColors(line); }
            else if (line.find("noeud") != std::string::npos) {     m_nodeColor = getDataColors(line); }
            else if (line.find("construct") != std::string::npos) { m_constructColor = getDataColors(line); }
            else if (line.find("in") != std::string::npos) {        m_startColor = getDataColors(line); }
            else if (line.find("out") != std::string::npos) {       m_endColor = getDataColors(line); }
        }
        file.close();
    } else {
        spdlog::critical("[ITD] Unable to open file");
    }

    /* --------------------------- Read Png file ------------------------- */
    int imgWidth, imgHeight, imgChannels;
    unsigned char *image = stbi_load(m_mapPath.c_str(), &imgWidth, &imgHeight, &imgChannels, STBI_rgb);
    if (nullptr == image) {
        spdlog::critical("Echec du chargement de l'image");
        debug_break();
    }

    // TODO construct adjency list from nodes, start and endpoints, with path color to know who is connected to who and in which order
    glm::vec3 color = glm::vec3(0);
    for (int y = 0; y < imgHeight; y++) {
        for (int x = 0; x < imgWidth; x++) {
            color = getPixelColors(image, imgWidth, x, y);
            if (color == m_nodeColor) {
                spdlog::info("Found node at x: {} y: {}", x ,y);
            } else if (color == m_startColor) {
                
            } else if (color == m_endColor) {

            } else if (color == m_pathColor) {

            }
        }
    }
    stbi_image_free(image);
}

Map::~Map() {
}

glm::vec3 Map::getPixelColors(unsigned char* image, int imageWidth, int x, int y) {
    glm::vec3 pixel;
    pixel.r = image[3 * (y * imageWidth + x) + 0];
    pixel.g = image[3 * (y * imageWidth + x) + 1];
    pixel.b = image[3 * (y * imageWidth + x) + 2];
    return pixel;
}

float Map::getData(std::string line) {
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

glm::vec3 Map::getDataColors(std::string line) {
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
