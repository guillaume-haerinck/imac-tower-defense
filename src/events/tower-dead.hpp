#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct TowerDead {
		TowerDead(glm::vec2 position) : position(position) {}

		glm::vec2 position;
	};
}