#pragma once

namespace cmpt {
struct AnimationDark {
	//Entity is darkened proportionaly to its animated.age
	//If bForward is true : entity brightens, otherwise it darkens
	AnimationDark(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}