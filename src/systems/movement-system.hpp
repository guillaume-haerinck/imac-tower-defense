#pragma once

#include <entt/entt.hpp>

class MovementSystem {
public:
    MovementSystem();
    ~MovementSystem();

    void update(entt::DefaultRegistry& registry, double deltatime);
};
