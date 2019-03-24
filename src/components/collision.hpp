#pragma once

#include <BulletCollision/CollisionShapes/btCollisionShape.h>

namespace cmpt {
struct Collision {
    Collision(btCollisionShape* collisionShape, bool isTrigger)
    : collisionShape(collisionShape), isTrigger(isTrigger) {}

    btCollisionShape* collisionShape;
    bool isTrigger;
};
}
