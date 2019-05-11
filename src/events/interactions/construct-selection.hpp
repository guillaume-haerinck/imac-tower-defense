#pragma once

enum ConstructibleType {
	TOWER_BASIC,
	MIRROR_BASIC
};

namespace evnt {
	struct ConstructSelection {
		ConstructSelection(ConstructibleType type) : type(type) {}

		ConstructibleType type;
	};
}
