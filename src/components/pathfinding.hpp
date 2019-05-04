#pragma once

#include "core/map/map.hpp"

namespace cmpt {
	struct Pathfinding {
		Pathfinding(Map* map, int startNode) : map(map), currentTarget(startNode), previousNode(-1){}
		int previousNode;
		int currentTarget;
		Map* map;
	};
}