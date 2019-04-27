#include "graph.hpp"

#include <spdlog/spdlog.h>

Graph::Graph() {

}

Graph::~Graph() {

}

int Graph::getNodesCount() {
	return this->nodes.size();
}

graphNode Graph::getNode(int nodeIndex) {
	return nodes.at(nodeIndex);
}

graphNodeList* Graph::getNeighbours(int nodeIndex) {
	return adjencyLists.at(nodeIndex);
}

int Graph::nodeIndex(int x, int y) {
	for (int k = 0; k < nodes.size(); ++k) {
		if (nodes.at(k).x == x && nodes.at(k).y == y) {
			return k;
		}
	}
	return -1;
}

bool Graph::isNeighbourOf(int node, int potentialNeighbour) {
	graphNodeList* list = adjencyLists.at(node);
	while (list != nullptr) {
		if (list->nodeIndex == potentialNeighbour) {
			return true;
		}
		list = list->next;
	}
	return false;
}

//Returns the index at which the node was inserted
int Graph::addNode(int x, int y) {
	nodes.push_back(graphNode(x, y));
	adjencyLists.push_back(nullptr);
	return getNodesCount()-1;
}

void Graph::addNodeToList(graphNodeList** list, int value) {
	//Create new cell
	graphNodeList* newCell = (graphNodeList*)malloc(sizeof(graphNodeList));
	newCell->nodeIndex = value;
	//Add cell to list
	if (*list == nullptr) {
		newCell->next = nullptr;
		*list = newCell;
	}
	else {
		newCell->next = (*list)->next;
		(*list)->next = newCell;
	}
}

void Graph::addNeighbourTo(int node, int neighbour) {
	addNodeToList(&adjencyLists.at(node), neighbour);
}

void Graph::addNeighbourTo(int node, int neighbour, bool checkRepetitions) {
	if (!(checkRepetitions && isNeighbourOf(node, neighbour))) {
		addNodeToList(&adjencyLists.at(node), neighbour);
	}
}

void Graph::addNeighbouring(int node1, int node2) {
	addNeighbourTo(node1, node2);
	addNeighbourTo(node2, node1);
}

void Graph::addNeighbouring(int node1, int node2, bool checkRepetitions) {
	addNeighbourTo(node1, node2, checkRepetitions);
	addNeighbourTo(node2, node1, checkRepetitions);
}