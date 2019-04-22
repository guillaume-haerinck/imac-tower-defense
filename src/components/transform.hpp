#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace cmpt {
struct Transform {
    Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::quat rotation = glm::quat(1, 0, 0, 0))
    : position(position), scale(scale), rotation(rotation) {}

    glm::vec3 position;
	glm::vec3 scale;
    glm::quat rotation;
};
}
