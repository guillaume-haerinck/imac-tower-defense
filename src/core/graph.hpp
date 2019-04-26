#pragma once
#include <vector>

struct graphNode {
	graphNode(int x, int y) : x(x), y(y) {}

	int x;
	int y;
};

struct graphNodeList {
	graphNode* node;
	graphNodeList* next;
};

class Graph {
	public :
		Graph();
		~Graph();
		int getNodesCount();
		void addNode();
	private :
		int nodesCount;
		std::vector<graphNodeList> adjencyLists;
};