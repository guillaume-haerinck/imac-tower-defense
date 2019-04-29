#pragma once
#include "map/map.hpp"

namespace cmpt {
	struct Pathfinding {
		Pathfinding(Map* map, int startNode) : map(map), currentTarget(startNode) {}
		int currentTarget;
		Map* map;
	};
}