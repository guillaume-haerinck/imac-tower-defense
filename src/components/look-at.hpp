#pragma once

namespace cmpt {
	struct LookAt {
		LookAt(unsigned int entityId) : entityId(entityId) {}

		unsigned int entityId;
	};
}
