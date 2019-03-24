#pragma once

#include <entt/entt.hpp>
#include <btBulletDynamicsCommon.h>

class PhysicSystem {
private:

public:
    PhysicSystem();
    ~PhysicSystem();

    void update(entt::DefaultRegistry& registry, double deltatime, btDiscreteDynamicsWorld& dynamicsWorld);
};
