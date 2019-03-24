#pragma once

#include <Box2D/Box2D.h>

namespace cmpt {
struct RigidBody {
    RigidBody(b2Body* body, b2FixtureDef* collider) : body(body), collider(collider) {}

    b2Body* body;
    b2FixtureDef* collider;
};
}
