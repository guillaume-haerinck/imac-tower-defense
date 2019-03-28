#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

class PhysicSystem {
private:

public:
    PhysicSystem();
    ~PhysicSystem();

    void update(entt::DefaultRegistry& registry, double deltatime, b2World* physicWorld);
};
