#pragma once

/* ask for the transform component to build rigid body */
#include <btBulletDynamicsCommon.h>
#include <entt/entt.hpp>
#include "components/rigid-body.hpp"
#include "components/transform.hpp"
#include "components/collision.hpp"

class RigidBodyFactory {
public:
    RigidBodyFactory(entt::DefaultRegistry& registry, btDiscreteDynamicsWorld& dynamicsWorld);
    ~RigidBodyFactory();

    cmpt::RigidBody createStatic(cmpt::Transform transform, cmpt::Collision collider);
    cmpt::RigidBody createDynamic(cmpt::Transform transform, cmpt::Collision collider, btScalar mass, btVector3 localInertia);

private:
    btDiscreteDynamicsWorld& m_dynamicsWorld;
    entt::DefaultRegistry& m_registry;
};

