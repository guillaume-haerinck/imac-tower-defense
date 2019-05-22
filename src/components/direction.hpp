#pragma once

#include <glm/glm.hpp>

namespace cmpt {
	struct Direction {
		//Entity moves in the "dir" direction
		Direction(float angle){
			dir = glm::vec2(cos(angle), sin(angle));
		}

		glm::vec2 dir;
	};
}
