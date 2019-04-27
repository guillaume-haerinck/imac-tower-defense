#pragma once

#include <entt/entt.hpp>

#include "system.hpp"

class AnimationSystem : public System {
public:
    AnimationSystem(entt::DefaultRegistry& registry);
    void update(double deltatime);
};
