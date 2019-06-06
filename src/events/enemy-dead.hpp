#pragma once

#include <glm/glm.hpp>
#include "entity-factories/enemy-factory.hpp"

namespace evnt {
	struct EnemyDead {
		EnemyDead(glm::vec2 position, EnemyType type) : position(position), type(type) {}

		glm::vec2 position;
		EnemyType type;
	};
}