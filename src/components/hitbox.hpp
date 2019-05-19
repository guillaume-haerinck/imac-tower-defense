#pragma once

namespace cmpt {
	struct Hitbox {
		Hitbox(float radius) : radius(radius) {
			radiusSq = radius * radius;
		}

		float radius;
		float radiusSq;
	};
}