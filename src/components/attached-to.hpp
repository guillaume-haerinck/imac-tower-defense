#pragma once

#include <glm/glm.hpp>

namespace cmpt {
		struct AttachedTo {
		AttachedTo(unsigned int entityId) : entityId(entityId) {}

		unsigned int entityId;
	};
}
