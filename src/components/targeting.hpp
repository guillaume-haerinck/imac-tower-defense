#pragma once

#include "spdlog/spdlog.h"

namespace cmpt {
	struct Targeting {
		Targeting(unsigned int targetId, float targetingRange) : targetId(targetId), targetingRange(targetingRange){}

		unsigned int targetId;
		float targetingRange;
	};
}
