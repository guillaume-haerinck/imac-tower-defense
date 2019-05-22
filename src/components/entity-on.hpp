#pragma once

namespace cmpt {
	struct EntityOnTile {
		//Gives the id of the building constructed on the tile that owns this component
		EntityOnTile(unsigned int entityId) : entityId(entityId) {}

		unsigned int entityId;
	};
}
