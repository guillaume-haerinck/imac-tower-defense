#pragma once

#include "core/level/level.hpp"

namespace cmpt {
	struct Pathfinding {
		//Move towards a node and pick a new node when it reaches its target
		Pathfinding(Level* level, int startNode) : level(level), currentTarget(startNode), previousNode(-1){}
		int previousNode;
		int currentTarget;
		Level* level;
	};
}