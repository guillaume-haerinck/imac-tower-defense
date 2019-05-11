#pragma once

#include <glm/glm.hpp>

namespace cmpt {
	struct Direction {
		Direction(float angle){
			dir = glm::vec2(cos(angle), sin(angle));
		}

		glm::vec2 dir;
	};
}
