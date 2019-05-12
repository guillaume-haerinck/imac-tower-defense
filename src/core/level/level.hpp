#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "entity-factories/tile-factory.hpp"
#include "graph.hpp"

class Level {
public:
	Level(entt::DefaultRegistry& registry, unsigned int levelNumber, glm::vec2& viewTranslation, float& viewScale);
	~Level();

	// Position interpolation
	glm::vec2 windowToGrid(float x, float y) const;
	glm::vec2 projToGrid(float x, float y) const;
	glm::vec2 gridToProj(unsigned int x, unsigned int y) const;

	// Getters
	std::uint32_t getTile(unsigned int x, unsigned int y) const;
	std::uint32_t getTileFromProjCoord(float x, float y) const;

	std::uint32_t getEntityOnTileFromProjCoord(float x, float y) const;

	glm::vec2 getNodePosition(int nodeIndex);
	unsigned int getGridWidth() const;
	unsigned int getGridHeight() const;
	Graph* getGraph() const;
	Graph* getPathfindingGraph() const;

	// Setters
	void setLevel(unsigned int number);

	// Debug drawing
	void drawGraph();
	void drawGrid();

private:
	// Read Itd and PPM
	glm::vec3 getPixelColorFromImage(unsigned char* image, int imageWidth, int x, int y);
	glm::vec3 getColorFromString(std::string line);
	float getNumberFromString(std::string line);

	// Graph construction
	bool isPath(unsigned char* image, int imageWidth, int imageHeight, int x, int y);
	bool isStraightLine(unsigned char* image, int imageWidth, int imageHeight, int x, int y);
	void lookForNodes(unsigned char* image, int imageWidth, int imageHeight, int parentNodeIndex, int x, int y, int xDir, int yDir, int travelLength);
	
	// Pathfinding graph
	void constructPathfindingGraph();

private:
	// .ITD file data
	glm::vec3 m_pathColor;
	glm::vec3 m_nodeColor;
	glm::vec3 m_startColor;
	glm::vec3 m_endColor;
	glm::vec3 m_constructColor;
	std::string m_mapPath;
	std::string m_itdPath;
	float m_energy;

	// Level details
	std::vector<unsigned int> m_grid;
	unsigned int m_gridWidth;
	unsigned int m_gridHeight;

	// Aggregation
	entt::DefaultRegistry& m_registry;
	TileFactory m_tileFactory;

	// External info
	const glm::vec2& m_viewTranslation;
	const float& m_viewScale;

	// Graph data
	Graph* m_graph;
	Graph* m_pathfindingGraph;
};
