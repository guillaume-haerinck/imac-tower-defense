#pragma once

namespace evnt {
	struct BeginContact {
		BeginContact(unsigned int entity1, unsigned int entity2) : entity1(entity1), entity2(entity2) {}

		unsigned int entity1;
		unsigned int entity2;
	};
}
