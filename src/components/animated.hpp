#pragma once

namespace cmpt {
	struct Animated {
		//Keeps track of an animation's age (a.k.a. time elapsed since the beginning of the animation)
		//Goes along with an "animation-[something]" component
		Animated(float duration, bool bDestroyAfterAnimation = false)
			: age(0), duration(duration), bDestroyAfterAnimation(bDestroyAfterAnimation) {}

		float age;
		float duration;
		bool bDestroyAfterAnimation;
	};
}
