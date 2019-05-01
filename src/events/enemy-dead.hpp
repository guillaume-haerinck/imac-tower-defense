#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct EnnemyDead {
		EnnemyDead(glm::vec2 position) : position(position) {}

		glm::vec2 position;
	};
}