#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct ProjectileHit {
		ProjectileHit(unsigned int targetId, glm::vec2 position, float damage) : targetId(targetId), position(position), damage(damage) {}

		unsigned int targetId;
		glm::vec2 position;
		float damage;
	};
}