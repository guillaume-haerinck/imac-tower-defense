#pragma once

#include "glm/glm.hpp"

namespace cmpt {
struct TransformTile {
	TransformTile(glm::vec2 tilePosition = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1.0f), float rotation = 0.0f)
	: tilePosition(tilePosition), scale(scale), rotation(rotation) {}

	glm::vec2 tilePosition;
	glm::vec2 scale;
	float rotation;
};
}
