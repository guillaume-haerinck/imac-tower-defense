#pragma once

enum class ConstructibleType {
	TOWER_LASER,
	TOWER_SLOW,
	MIRROR_BASIC
};

namespace evnt {
	struct ConstructSelection {
		ConstructSelection(ConstructibleType type) : type(type) {}

		ConstructibleType type;
	};
}
