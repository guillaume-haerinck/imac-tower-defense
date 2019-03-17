#pragma once

#include <entityx/entityx.h>

class AnimationSystem {
public:
    AnimationSystem();
    ~AnimationSystem();

    void update(entityx::EntityX& registry, double deltatime);
};
