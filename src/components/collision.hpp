#pragma once

namespace cmpt {
struct Collision {
    Collision(bool isTrigger)
    : isTrigger(isTrigger) {}

    //btCollisionShape* collisionShape;
    bool isTrigger;
};
}
