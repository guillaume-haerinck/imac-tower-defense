#pragma once

namespace cmpt {
	struct MoveTowardsMouse {
		//Move away from its position, towards the mouse
		//Cannot go further than maxDist in the mouse direction
		MoveTowardsMouse(float maxDist) : maxDist(maxDist) {}
		float maxDist;
	};
}
