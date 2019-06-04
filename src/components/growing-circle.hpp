#pragma once

namespace cmpt {
struct GrowingCircle {
	//Renders the entity as a circle which radius grows over time
	//Entity must have an "age" component
	GrowingCircle(float growthSpeed) : growthSpeed(growthSpeed) {}

	float growthSpeed;
};
}
