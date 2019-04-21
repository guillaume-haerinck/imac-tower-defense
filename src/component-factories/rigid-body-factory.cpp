#include "rigid-body-factory.hpp"

RigidBodyFactory::RigidBodyFactory(entt::DefaultRegistry& registry, b2World* physicWorld) 
: m_registry(registry), m_physicWorld(physicWorld) {}

RigidBodyFactory::~RigidBodyFactory() {
    for (int i = 0; i < m_colliders.size() - 1; i++) {
        delete m_colliders[i];
    }
}

cmpt::RigidBody RigidBodyFactory::create(b2BodyType type, cmpt::Transform transform, b2FixtureDef* collider) {
    b2BodyDef bodyDef; // Will be cloned so can go out of scope
	bodyDef.type = type;
	bodyDef.position.Set(transform.position.x, transform.position.y);
    bodyDef.angle = transform.rotation.z;
	b2Body* body = m_physicWorld->CreateBody(&bodyDef);
    body->CreateFixture(collider->shape, 0.0f);

    m_colliders.push_back(collider);
    cmpt::RigidBody rb(body, collider);
    return rb;
}
