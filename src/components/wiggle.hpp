#pragma once

#include <glm/glm.hpp>
#include "services/locator.hpp"
#include "services/random/i-random.hpp"

namespace cmpt {
	struct Wiggle {
		Transform(){
			IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
			noiseOffset = randomService.random(500);
		}

		float noiseOffset;
	};
}
