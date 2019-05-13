#pragma once

namespace cmpt {
	struct Velocity {
		Velocity(float velocity)
			: velocity(velocity), velMultiplier(1), multiplierAge(0) ,multiplierLifespan(1) {}

		float velocity;
		float velMultiplier;
		float multiplierAge;
		float multiplierLifespan;
	};
}
