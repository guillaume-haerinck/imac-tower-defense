#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include "components/rigid-body.hpp"
#include "components/transform.hpp"

class RigidBodyFactory {
public:
    RigidBodyFactory(entt::DefaultRegistry& registry, b2World& physicWorld);
    ~RigidBodyFactory();

    cmpt::RigidBody create(b2BodyType type, cmpt::Transform transform, b2Shape* colliderShape, b2FixtureDef* collider);

private:
    entt::DefaultRegistry& m_registry;
    b2World& m_physicWorld;
    std::vector<b2FixtureDef*> m_colliders;
    std::vector<b2Shape*> m_colliderShapes;
};
