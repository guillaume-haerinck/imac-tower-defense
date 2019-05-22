#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct EntityDamaged {
		EntityDamaged(std::uint32_t targetId, float damage) : entity(targetId), damage(damage) {}

		std::uint32_t entity;
		float damage;
	};
}