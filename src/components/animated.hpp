#pragma once

namespace cmpt {
	struct Animated {
		Animated(float duration, bool bDestroyAfterAnimation = false)
			: age(0), duration(duration), bDestroyAfterAnimation(bDestroyAfterAnimation) {}

		float age;
		float duration;
		bool bDestroyAfterAnimation;
	};
}
