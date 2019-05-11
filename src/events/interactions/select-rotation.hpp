#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct SelectRotation {
		SelectRotation(glm::vec2 mousePos) : mousePos(mousePos) {}

		glm::vec2 mousePos;
	};
}
