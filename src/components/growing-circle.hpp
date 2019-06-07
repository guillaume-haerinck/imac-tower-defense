#pragma once

namespace cmpt {
struct GrowingCircle {
	//Renders the entity as a circle which radius grows over time
	//Entity must have an "age" component
	GrowingCircle(float growthSpeed, float maxRadius) : growthSpeed(growthSpeed), maxRadius(maxRadius) {
		IRandom& random = entt::ServiceLocator<IRandom>::ref();
		noiseSeed = random.random(1,2);
	}

	float growthSpeed;
	float maxRadius;
	float noiseSeed;
};
}
