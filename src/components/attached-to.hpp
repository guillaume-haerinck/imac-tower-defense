#pragma once

#include <glm/glm.hpp>

namespace cmpt {
struct AttachedTo {
	//Entity will inherit proporties such as position and colour from the main entity
	AttachedTo(unsigned int mainEntity) : mainEntity(mainEntity) {}

	unsigned int mainEntity;
};
}
