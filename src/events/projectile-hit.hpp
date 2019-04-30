#pragma once

namespace evnt {
	struct ProjectileHit {
		ProjectileHit(unsigned int targetId, float damage) : targetId(targetId), damage(damage) {}

		unsigned int targetId;
		float damage;
	};
}