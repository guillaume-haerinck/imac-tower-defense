#pragma once

#include "glm/glm.hpp"

namespace cmpt {
struct TransformTile {
	TransformTile(glm::vec2 tileIndex = glm::vec2(0, 0), glm::vec2 sizeInTileUnit = glm::vec2(1.0f), float rotation = 0.0f, int zIndex = 0)
	: tileIndex(tileIndex), sizeInTileUnit(sizeInTileUnit), rotation(rotation), zIndex(zIndex) {}

	glm::vec2 tileIndex;
	glm::vec2 sizeInTileUnit;
	float rotation;
	int zIndex;
};
}
