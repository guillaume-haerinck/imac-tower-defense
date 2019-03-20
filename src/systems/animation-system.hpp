#pragma once

#include <entt/entt.hpp>

class AnimationSystem {
public:
    AnimationSystem();
    ~AnimationSystem();

    void update(entt::DefaultRegistry& registry, double deltatime);
};
