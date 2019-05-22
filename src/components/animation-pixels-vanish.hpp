#pragma once

namespace cmpt {
struct AnimationPixelsVanish {
	//Some pixels are not drawn. The number of undrawn pixels is proportional to the animated.age
	//If bForward is true : there is less an less pixels drawn, otherwise there is more and more
	AnimationPixelsVanish(bool bForward)
    : bForward(bForward){}

	bool bForward;
};
}