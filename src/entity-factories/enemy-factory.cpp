#include "enemy-factory.hpp"

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"
#include "components/sprite-animation.hpp"

EnemyFactory::EnemyFactory(entt::DefaultRegistry& registry)
: m_registry(registry) {
}

EnemyFactory::~EnemyFactory() {}

void EnemyFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createAtlas("res/images/spritesheets/spaceman-196x196.png", glm::vec2(1.0f), glm::vec2(196, 196)));
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec3(posX * WIN_RATIO, posY, 0.0f), glm::vec3(TILE_SIZE, TILE_SIZE, 0.0f), glm::quat(1, 0, 0, 0));
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 0);
}

/*
// Exemple setup physics
b2PolygonShape myColliderShape1;
myColliderShape1.SetAsBox(10.0f, 10.0f);
b2FixtureDef* myCollider1 = new b2FixtureDef(); // TODO use smart pointer for collider deletion
myCollider1->density = 1.0f;
myCollider1->friction = 0.3f;
myCollider1->shape = &myColliderShape1; // Will be cloned so can go out of scope
registry.assign<cmpt::RigidBody>(myEntity, rigidBodyFactory.create(b2_staticBody, myTransform1, myCollider1));
*/
