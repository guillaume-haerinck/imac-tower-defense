#pragma once

#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace cmpt {
struct RigidBody {
    RigidBody() {}

    btRigidBody* rb;
};
}
