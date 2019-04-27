#pragma once
#include <vector>

struct graphNode { graphNode(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

struct graphNodeList { graphNodeList(int nodeIndex) : nodeIndex(nodeIndex) , next(nullptr) {}
	int nodeIndex;
	graphNodeList* next;
};

class Graph {
	public :
		Graph();
		~Graph();
		int getNodesCount();
		graphNode getNode(int nodeIndex);
		graphNodeList* getNeighbours(int nodeIndex);
		int addNode( int x , int y );
		void addNeighbouring(int node1, int node2);
		void addNeighbouring(int node1, int node2, bool checkRepetitions);
		int nodeIndex(int x, int y);
		bool isNeighbourOf(int node, int potentialNeighbour);

	private :
		std::vector<graphNode> nodes;
		std::vector<graphNodeList*> adjencyLists;
		void addNodeToList(graphNodeList** list, int value);
		void addNeighbourTo(int node, int neighbour);
		void addNeighbourTo(int node, int neighbour, bool checkRepetitions);
};