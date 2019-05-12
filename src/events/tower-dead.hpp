#pragma once

#include <glm/glm.hpp>

// TODO mix delete-entity, enemy dead and tower-dead into one event

namespace evnt {
	struct TowerDead {
		TowerDead(glm::vec2 position) : position(position) {}

		glm::vec2 position;
	};
}