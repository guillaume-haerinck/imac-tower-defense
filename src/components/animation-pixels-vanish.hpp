#pragma once

namespace cmpt {
struct AnimationPixelsVanish {
	//Some pixels are not drawn. The number of undrawn pixels is proportional to the animated.age
	//If bForward is true : there is more and more pixels drawn, otherwise there is less and less
	AnimationPixelsVanish(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}