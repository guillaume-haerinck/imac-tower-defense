#pragma once

namespace evnt {
	struct DeleteEntity {
		DeleteEntity(int entityId) : entityId(entityId) {}

		int entityId;
	};
}
