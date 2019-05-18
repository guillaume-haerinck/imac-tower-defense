#pragma once

namespace cmpt {
	struct Animated {
		Animated(float duration)
			: age(0), duration(duration) {}

		float age;
		float duration;
	};
}
