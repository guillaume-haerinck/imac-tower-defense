#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

// TODO set position in the grid ? The renderer then transform it to xy ?
// The scale is the number of tiles of the grid it takes, a int ?

namespace cmpt {
struct Transform {
    Transform(glm::vec3 scale = glm::vec3(1.0f), glm::vec3 position = glm::vec3(0.0f), glm::quat rotation = glm::quat()) 
    : scale(scale), position(position), rotation(rotation) {}

    glm::vec3 scale;
    glm::vec3 position;
    glm::quat rotation;
};
}
