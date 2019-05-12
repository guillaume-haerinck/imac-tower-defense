#pragma once

#include <glm/glm.hpp>

namespace cmpt {
	struct Shake {
		Shake() : offset(glm::vec2(0)) {}

		glm::vec2 offset;
	};
}
