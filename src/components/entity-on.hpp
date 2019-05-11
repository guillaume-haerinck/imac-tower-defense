#pragma once

// Used on tile to get the entity built on it
// TODO rename

namespace cmpt {
	struct EntityOn {
		EntityOn(unsigned int entityId) : entityId(entityId) {}

		unsigned int entityId;
	};
}
