#pragma once

#include "services/random/i-random.hpp"

namespace cmpt {
	struct Wiggle {
		Wiggle() {
			IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
			noiseOffset = randomService.random(500);
		}

		float noiseOffset;
		glm::vec2 latestMove;
	};
}
