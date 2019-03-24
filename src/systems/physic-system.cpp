#include "physic-system.hpp"

#include <iostream>
#include "components/rigid-body.hpp"
#include "components/transform.hpp"

PhysicSystem::PhysicSystem() {}

PhysicSystem::~PhysicSystem() {}

void PhysicSystem::update(entt::DefaultRegistry& registry, double deltatime, b2World& physicWorld) {
    registry.view<cmpt::RigidBody, cmpt::Transform>().each([&](auto entity, cmpt::RigidBody& rigidbody, cmpt::Transform& transform) {
        transform.position.x = rigidbody.body->GetPosition().x;
        transform.position.y = rigidbody.body->GetPosition().y;
        // TODO add angle
    });
}
