#pragma once

#include <glm/glm.hpp>

namespace cmpt {
	struct Transform {
		Transform(glm::vec2 position = glm::vec2(0.0f), int zIndex = 0, glm::vec2 scale = glm::vec2(1.0f), float rotation = 0.0f)
		: position(position), zIndex(zIndex), scale(scale), rotation(rotation) {}

		glm::vec2 position;
		glm::vec2 scale;
		float rotation;
		int zIndex;
	};
}
