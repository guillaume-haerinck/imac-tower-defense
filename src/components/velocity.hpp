#pragma once

namespace cmpt {
	struct Velocity {
		Velocity(float velocity)
			: velocity(velocity), velMultiplier(1), multiplierLifespan(0) {}

		float velocity;
		float velMultiplier;
		float multiplierLifespan;
	};
}
