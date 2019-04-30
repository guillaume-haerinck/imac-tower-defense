#include "rigid-body-factory.hpp"

RigidBodyFactory::RigidBodyFactory(b2World& physicWorld)
: m_physicWorld(physicWorld) {}

cmpt::RigidBody RigidBodyFactory::create(b2BodyType type, cmpt::Transform transform, b2FixtureDef* collider) {
    b2BodyDef bodyDef; // Will be cloned so can go out of scope
	bodyDef.type = type;
	bodyDef.position.Set(transform.position.x, transform.position.y);
    bodyDef.angle = transform.rotation;
	b2Body* body = m_physicWorld.CreateBody(&bodyDef);
    body->CreateFixture(collider);

    cmpt::RigidBody rb(body, collider);
    return rb;
}
