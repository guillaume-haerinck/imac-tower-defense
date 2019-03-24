#include "physic-system.hpp"

#include <stdio.h>
#include "components/rigid-body.hpp"
#include "components/transform.hpp"

PhysicSystem::PhysicSystem() {}

PhysicSystem::~PhysicSystem() {}

void PhysicSystem::update(entt::DefaultRegistry& registry, double deltatime, btDiscreteDynamicsWorld& dynamicsWorld) {
    for (int i = dynamicsWorld.getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld.getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
    }
    
    registry.view<cmpt::RigidBody, cmpt::Transform>().each([](auto entity, cmpt::RigidBody& rigidbody, cmpt::Transform& transform) {
        btTransform trans;
        trans = rigidbody.rb->getWorldTransform();
        transform.position = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
    });
}
