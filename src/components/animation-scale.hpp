#pragma once

namespace cmpt {
struct AnimationScale {
	AnimationScale(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}