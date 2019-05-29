#pragma once

namespace cmpt {
struct AnimationAlpha {
	//Entity is partially transparent
	//If bForward is true : entity is less and less transparent
	AnimationAlpha(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}