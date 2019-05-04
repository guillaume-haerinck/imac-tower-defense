#include "graph.hpp"

#include <limits>

#include "services/locator.hpp"
#include "services/random/i-random.hpp"

Graph::Graph() {

}

Graph::~Graph() {
	for (int i = 0; i < adjencyLists.size(); i++) {
		delete adjencyLists.at(i);
	}
}

int Graph::getNodesCount() {
	return this->nodes.size();
}

graphNode Graph::getNode(int nodeIndex) {
	return nodes.at(nodeIndex);
}

std::vector<graphEdge>* Graph::getNeighbours(int nodeIndex) {
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
	std::vector<graphEdge>* list = adjencyLists.at(node);
	for (int i = 0; i < list->size(); ++i) {
		if ( list->at(i).neighbourIndex == potentialNeighbour) {
			return true;
		}
	}
	return false;
}

//Returns the index at which the node was inserted
int Graph::addNode(int x, int y) {
	nodes.push_back(graphNode(x, y));
	adjencyLists.push_back( new std::vector<graphEdge> );
	return getNodesCount() - 1;
}

void Graph::addStartNode(int nodeIndex) {
	startNodeIndexes.push_back(nodeIndex);
}

void Graph::addEndNode(int nodeIndex) {
	endNodeIndex = nodeIndex;
}

std::vector<int> Graph::getStartNodes() {
	return startNodeIndexes;
}

int Graph::getStartNodeRandom() {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	return startNodeIndexes.at(randomService.randInt(startNodeIndexes.size()));
}

int Graph::getEndNode() {
	return endNodeIndex;
}

/*void Graph::addNodeToList(graphEdgeList** list, int value, float dist) {
	//Create new cell
	graphEdgeList* newCell = (graphEdgeList*)malloc(sizeof(graphEdgeList));
	newCell->edge.neighbourIndex = value;
	newCell->edge.dist = dist;
	//Add cell to list
	if (*list == nullptr) {
		newCell->next = nullptr;
		*list = newCell;
	}
	else {
		newCell->next = (*list)->next;
		(*list)->next = newCell;
	}
}*/

void Graph::addNeighbourTo(int node, int neighbour, float dist) {
	adjencyLists.at(node)->push_back(graphEdge(neighbour, dist));
}

void Graph::addNeighbourTo(int node, int neighbour, float dist, bool checkRepetitions) {
	if (!checkRepetitions || !isNeighbourOf(node, neighbour)) {
		addNeighbourTo(node, neighbour, dist);
	}
}

void Graph::addNeighbouring(int node1, int node2, float dist) {
	addNeighbourTo(node1, node2, dist);
	addNeighbourTo(node2, node1, dist);
}

void Graph::addNeighbouring(int node1, int node2, float dist, bool checkRepetitions) {
	addNeighbourTo(node1, node2, dist, checkRepetitions);
	addNeighbourTo(node2, node1, dist, checkRepetitions);
}

float Graph::distEstimator(int node1) {
	return distEstimator(node1, getEndNode());
}

float Graph::distEstimator(int node1, int node2) {
	return abs(getNode(node1).x - getNode(node2).x) + abs(getNode(node1).y - getNode(node2).y);
}

int Graph::pickNextNode(int currentNode, int previousNode) {
	std::vector<graphEdge>* neighbours = getNeighbours(currentNode);
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	float r = randomService.random();
	int i = 0;
	while (r > neighbours->at(i).dist) {
		r -= neighbours->at(i).dist;
		i++;
	}
	if (neighbours->at(i).neighbourIndex != previousNode || neighbours->at(i).dist > 0.999) {
		return neighbours->at(i).neighbourIndex;
	}
	else {
		return pickNextNode(currentNode, previousNode);
		//I hate to do it this way, but it is easy. . .
		//and it should be fine since there cannot be more than 4 neighbours and they (supposedely) will not have absurdely different weights
	}
}

/*int Graph::pickNextNode(int currentNode, int previousNode) {
	graphEdgeList* neighbours = getNeighbours(currentNode);
	float r = random();
	while ( r > neighbours->edge.dist && neighbours->edge.neighbourIndex != previousNode) {
		r -= neighbours->edge.dist;
		neighbours = neighbours->next;
	}
	if (neighbours->edge.neighbourIndex != previousNode) {
		return neighbours->edge.neighbourIndex;
	}
	else {
		float p = neighbours->edge.dist;
		if (p > 0.999) {
			return neighbours->edge.neighbourIndex;
		}
		else {
			neighbours = getNeighbours(currentNode);
			float scale = 1. / (1 - p);
			r = random();
			while (r > neighbours->edge.dist * scale || neighbours->edge.neighbourIndex != previousNode) {
				spdlog::info("{}", r);
				if (neighbours->edge.neighbourIndex != previousNode) {
					r -= neighbours->edge.dist * scale;
				}
				neighbours = neighbours->next;
			}
			return neighbours->edge.neighbourIndex;
		}
	}
}*/

/*std::vector<int> Graph::trajectory(int startNode, int endNode) {
	//Using Dijkstra algorithm
	//We start at endNode because the path will be built backward at the end and we want it to start at startNode

	//Initialize
	srand(time(NULL));
	std::vector<int> traj;
	int N = getNodesCount();
	float* dists = (float*)malloc(N * sizeof(float));
	int* prevNode = (int*)malloc(N * sizeof(int));
	bool* isDone = (bool*)malloc(N * sizeof(bool));
	for (int k = 0; k < N; ++k) {
		dists[k] = std::numeric_limits<float>::infinity();
		prevNode[k] = -1;
		isDone[k] = false;
	}
	dists[endNode] = 0;

	//Loop: 
	bool targetFound = false;
	int currentNode;
	while (!targetFound) {
		//Look for node with min dist to endNode
		currentNode = -1;
		for (int k = 0; k < N; ++k) {
			if (!isDone[k] && (currentNode == -1 || dists[k] < dists[currentNode])) {
				currentNode = k;
			}
		}
		isDone[currentNode] = true;
		//Check if finished
		targetFound = (currentNode == -1 || currentNode == startNode);
		//
		//Visit current node's neighbours
		graphEdgeList* neighbours = getNeighbours(currentNode);
		while (neighbours != nullptr) {
			float distTest = dists[currentNode] + neighbours->edge.dist;
			int neighbour = neighbours->edge.neighbourIndex;
			if (distTest < dists[neighbour] || (distTest == dists[neighbour] && static_cast <float> (rand()) / static_cast <float> (RAND_MAX) <0.5)) {
				dists[neighbour] = distTest;
				prevNode[neighbour] = currentNode;
			}
			neighbours = neighbours->next;
		}
	}
	//Reconstruct shortest path
	while (currentNode != -1) {
		//spdlog::info("Node at {} {}", getNode(currentNode).x, getNode(currentNode).y);
		traj.push_back(currentNode);
		currentNode = prevNode[currentNode];
	}
	return traj;
}
*/