#pragma once

#include "services/random/i-random.hpp"

namespace cmpt {
struct GrowingCircle {
	//Renders the entity as a circle which radius grows over time
	//Entity must have an "age" component
	GrowingCircle(float growthSpeed, float maxRadius) : growthSpeed(growthSpeed), maxRadius(maxRadius) {
		IRandom& random = entt::ServiceLocator<IRandom>::ref();
		noiseSeed = random.random(2);
	}

	float growthSpeed;
	float maxRadius;
	float noiseSeed;
};
}
