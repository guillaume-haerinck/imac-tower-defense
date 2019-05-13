#pragma once

#include <glm/glm.hpp>

namespace cmpt {
		struct tintColour {
			tintColour(glm::vec4 col) : col(col) {}

			glm::vec4 col;
	};
}
