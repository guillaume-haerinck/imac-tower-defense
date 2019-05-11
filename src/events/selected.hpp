#pragma once

namespace evnt {
	struct Selected {
		Selected(unsigned int entity) : entity(entity) {}

		unsigned int entity;
	};
}
