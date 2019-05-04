#pragma once
#include <vector>
#include <spdlog/spdlog.h>

struct graphNode {
	graphNode(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

struct graphEdge {
	graphEdge(int neighbourIndex, float dist) : neighbourIndex(neighbourIndex), dist(dist) {}
	int neighbourIndex;
	float dist;
};

class Graph {
public:
	Graph();
	~Graph();
	//Getters
	int getNodesCount();
	graphNode getNode(int nodeIndex);
	int nodeIndex(int x, int y);
	std::vector<int> getStartNodes();
	int getStartNodeRandom();
	int getEndNode();
	std::vector<graphEdge>* getNeighbours(int nodeIndex);
	//Setters
	int addNode(int x, int y); //Returns the index at which the node was inserted
	void addStartNode(int nodeIndex);
	void addEndNode(int nodeIndex);
	void addNeighbourTo(int node, int neighbour, float dist);
	void addNeighbourTo(int node, int neighbour, float dist, bool checkRepetitions);
	void addNeighbouring(int node1, int node2, float dist);
	void addNeighbouring(int node1, int node2, float dist, bool checkRepetitions);
	bool isNeighbourOf(int node, int potentialNeighbour);
	float distEstimator(int node1);
	float distEstimator(int node1, int node2);
	int pickNextNode(int currentNode, int previousNode); //WARNING : should only be used if it is a stochastic graph ! (i.e. wheights of edges starting from a given node always add up to 1)
	//std::vector<int> trajectory(int startNode, int endNode);

private:
	std::vector<graphNode> nodes;
	std::vector<int> startNodeIndexes;
	int endNodeIndex;
	std::vector<std::vector<graphEdge>*> adjencyLists;
	//void addNodeToList(graphEdgeList** list, int value, float dist);
};