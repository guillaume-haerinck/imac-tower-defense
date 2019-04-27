#pragma once

#include <Box2D/Box2D.h>

#include "components/rigid-body.hpp"
#include "components/transform.hpp"

class RigidBodyFactory {
public:
    RigidBodyFactory(b2World& physicWorld);

    cmpt::RigidBody create(b2BodyType type, cmpt::Transform transform, b2FixtureDef* collider);

private:
	b2World& m_physicWorld;
};
