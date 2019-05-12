#pragma once

#include "services/random/i-random.hpp"

namespace cmpt {
	struct Wiggle {
		Wiggle(float amplitude) : amplitude(amplitude) {
			IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
			noiseOffset = randomService.random(500);
		}

		float amplitude;
		float noiseOffset;
	};
}
