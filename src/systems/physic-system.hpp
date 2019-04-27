#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"

class PhysicSystem : public System {
public:
    PhysicSystem(entt::DefaultRegistry& registry);
    void update(double deltatime, b2World* physicWorld);
};
