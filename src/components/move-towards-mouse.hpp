#pragma once

namespace cmpt {
	struct MoveTowardsMouse {
		MoveTowardsMouse(float maxDist) : maxDist(maxDist) {}
		float maxDist;
	};
}
