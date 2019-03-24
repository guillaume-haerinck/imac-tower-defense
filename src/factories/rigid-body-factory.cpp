#include "rigid-body-factory.hpp"

RigidBodyFactory::RigidBodyFactory(entt::DefaultRegistry& registry, b2World& physicWorld) 
: m_registry(registry), m_physicWorld(physicWorld) {}

RigidBodyFactory::~RigidBodyFactory() {
    for (int i = 0; i < m_colliders.size() - 1; i++) {
        delete m_colliders[i];
    }

    for (int i = 0; i < m_colliderShapes.size() - 1; i++) {
        delete m_colliderShapes[i];
    }
}

cmpt::RigidBody RigidBodyFactory::create(b2BodyType type, cmpt::Transform transform, b2Shape* colliderShape, b2FixtureDef* collider) {
    collider->shape = colliderShape;
    b2BodyDef bodyDef; // Will be clone so can go out of scope
	bodyDef.type = type;
	bodyDef.position.Set(transform.position.x, transform.position.y);
	b2Body* body = m_physicWorld.CreateBody(&bodyDef);
    body->CreateFixture(colliderShape, 0.0f);

    m_colliders.push_back(collider);
    m_colliderShapes.push_back(colliderShape);
    cmpt::RigidBody rb(body, collider);
    return rb;
}
