#pragma once

#include "glm/glm.hpp"

namespace cmpt {
struct Collision {
    Collision(glm::vec3 size = glm::vec3(1.0f), bool isTrigger = false) 
    : size(size), isTrigger(isTrigger) {}

    glm::vec3 size;
    bool isTrigger;
};
}
