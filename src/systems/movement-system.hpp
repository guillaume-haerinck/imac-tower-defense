#pragma once

#include <entityx/entityx.h>

class MovementSystem {
public:
    MovementSystem();
    ~MovementSystem();

    void update(entityx::EntityX& registry, double deltatime);
};
