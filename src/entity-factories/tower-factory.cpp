#include "tower-factory.hpp"

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>

#include "core/tags.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"
#include "components/look-at.hpp"
#include "components/shoot-at.hpp"
#include "components/rigid-body.hpp"
#include "components/targeting.hpp"

#include "core/random.hpp"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry, b2World& physicWorld)
: Factory(registry), m_rigidBodyFactory(physicWorld) {}

void TowerFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createSingle("res/images/textures/arrow.png", glm::vec2(8.0f)));
	m_registry.assign<renderTag::Single>(myEntity);
	const cmpt::Transform transform(glm::vec2(posX, posY));
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY));
	m_registry.assign<cmpt::LookAt>(myEntity);
	m_registry.assign<cmpt::ShootAt>(myEntity,randInt(60,180));
	m_registry.assign<cmpt::Targeting>(myEntity,0);
	// Setup physic
	// Cannot both follow and have physic, create another entity here only dedicated to collision ?
	/*
	b2CircleShape colliderShape;
	colliderShape.m_radius = 20.0f;
	b2FixtureDef* collider = new b2FixtureDef(); // Use unique smart pointer ?
	collider->density = 1.0f;
	collider->friction = 0.3f;
	collider->shape = &colliderShape; // Will be cloned so can go out of scope
	m_registry.assign<cmpt::RigidBody>(myEntity, m_rigidBodyFactory.create(b2_staticBody, transform, collider));
	*/
}
