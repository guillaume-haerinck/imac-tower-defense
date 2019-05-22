#pragma once

namespace cmpt {
struct AnimationScale {
	//Entity is scaled proportionaly to its animated.age
	//If bForward is true : entity grows, otherwise it shrinks
	AnimationScale(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}