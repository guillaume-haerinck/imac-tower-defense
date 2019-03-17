#pragma once

#include "glm/glm.hpp"

namespace cmpt {
struct Motion {
    Motion(glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 acceleration = glm::vec3(0.0f)) 
    : velocity(velocity), acceleration(acceleration) {}

    glm::vec3 velocity;
    glm::vec3 acceleration;
};
}
