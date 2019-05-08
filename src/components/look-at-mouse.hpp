#pragma once

namespace cmpt {
	struct LookAtMouse {
		LookAtMouse(float angleOffset = 0) : angleOffset(angleOffset) {}
		float angleOffset;
	};
}
