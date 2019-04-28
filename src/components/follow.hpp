#pragma once

namespace cmpt {
	struct Follow {
		Follow(unsigned int targetId) : targetId(targetId) {}

		unsigned int targetId;
	};
}
