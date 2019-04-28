#pragma once
#include <vector>
#include <spdlog/spdlog.h>

struct graphNode { graphNode(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

struct graphEdge {graphEdge( int neighbourIndex, float dist) : neighbourIndex(neighbourIndex), dist(dist) {}
	int neighbourIndex;
	float dist;
};

struct graphEdgeList {graphEdgeList(graphEdge edge) : edge(edge) , next(nullptr) {}
	graphEdge edge;
	graphEdgeList* next;
};

class Graph {
	public :
		Graph();
		~Graph();
		int getNodesCount();
		graphNode getNode(int nodeIndex);
		graphEdgeList* getNeighbours(int nodeIndex);
		int addNode( int x , int y );
		void addStartNode(int nodeIndex);
		void addEndNode(int nodeIndex);
		std::vector<int> getStartNodes();
		int getEndNode();
		void addNeighbouring(int node1, int node2, float dist);
		void addNeighbouring(int node1, int node2, float dist, bool checkRepetitions);
		int nodeIndex(int x, int y);
		bool isNeighbourOf(int node, int potentialNeighbour);
		std::vector<int> trajectory(int startNode, int endNode);

	private :
		std::vector<graphNode> nodes;
		std::vector<int> startNodeIndexes;
		int endNodeIndex;
		std::vector<graphEdgeList*> adjencyLists;
		void addNodeToList(graphEdgeList** list, int value, float dist);
		void addNeighbourTo(int node, int neighbour, float dist);
		void addNeighbourTo(int node, int neighbour, float dist, bool checkRepetitions);
};