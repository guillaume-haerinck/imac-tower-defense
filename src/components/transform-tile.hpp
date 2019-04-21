#pragma once

#include "glm/glm.hpp"

namespace cmpt {
struct TransformTile {
	TransformTile(glm::vec2 tilePosition, float rotation, glm::vec2 scale) : tilePosition(tilePosition), rotation(rotation), scale(scale) {}

	glm::vec2 tilePosition;
	float rotation;
	glm::vec2 scale;
};
}
