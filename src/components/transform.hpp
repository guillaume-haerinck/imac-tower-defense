#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace cmpt {
struct Transform {
    glm::vec3 scale;
    glm::vec3 position;
    glm::quat rotation;
};
}
