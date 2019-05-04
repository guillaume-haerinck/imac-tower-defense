#pragma once

#include "core/level/level.hpp"

namespace cmpt {
	struct Pathfinding {
		Pathfinding(Level* level, int startNode) : level(level), currentTarget(startNode), previousNode(-1){}
		int previousNode;
		int currentTarget;
		Level* level;
	};
}