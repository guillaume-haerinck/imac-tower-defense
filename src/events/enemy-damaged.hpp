#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct EnemyDamaged {
		EnemyDamaged(std::uint32_t targetId, glm::vec2 position, float damage) : targetId(targetId), position(position), damage(damage) {}

		std::uint32_t targetId;
		glm::vec2 position;
		float damage;
	};
}