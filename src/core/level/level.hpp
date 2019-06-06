#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "entity-factories/tile-factory.hpp"
#include "entity-factories/tower-factory.hpp"
#include "entity-factories/mirror-factory.hpp"
#include "core/progression.hpp"
#include "graph.hpp"

class Level {
public:
	Level(entt::DefaultRegistry& registry, Progression& progression, unsigned int levelNumber, glm::vec2& viewTranslation, float& viewScale);
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
	bool doesLevelExist(unsigned int number);

	// Update tile system
	void updateTileSystem() const;

	// Debug drawing
	void drawGraph();
	void drawGrid();

private:
	// Read Itd and PPM
	void readMapImage();
	glm::vec3 getPixelColorFromImage(unsigned char* image, int imageWidth, int x, int y);
	glm::vec3 getVec3FromString(std::string line);
	float getFloatFromString(std::string line);
	int getIntFromString(std::string line);

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
	std::string m_backgroundImgPath;

	// Level details
	std::vector<std::uint32_t> m_grid;
	unsigned int m_gridWidth;
	unsigned int m_gridHeight;

	// Aggregation
	entt::DefaultRegistry& m_registry;
	Progression& m_progression;
	TileFactory m_tileFactory;
	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;

	// External info
	const glm::vec2& m_viewTranslation;
	const float& m_viewScale;

	// Graph data
	Graph* m_graph;
	Graph* m_pathfindingGraph;
};
