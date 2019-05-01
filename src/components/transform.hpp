#pragma once

#include <glm/glm.hpp>

namespace cmpt {
struct Transform {
    Transform(glm::vec2 position = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(1.0f), float rotation = 0.0f, int zIndex = 0)
    : position(position), scale(scale), rotation(rotation), zIndex(zIndex) {}

    glm::vec2 position;
	glm::vec2 scale;
	float rotation;
	int zIndex;
};
}
