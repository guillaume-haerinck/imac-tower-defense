#pragma once

namespace cmpt {
struct AnimationPixelsVanish {
	AnimationPixelsVanish(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}