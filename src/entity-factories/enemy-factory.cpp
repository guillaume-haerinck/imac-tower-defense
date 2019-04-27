#include "enemy-factory.hpp"

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"
#include "components/sprite-animation.hpp"

EnemyFactory::EnemyFactory(entt::DefaultRegistry& registry) : Factory(registry) {}

void EnemyFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createAtlas("res/images/spritesheets/spaceman-196x196.png", glm::vec2(13.0f), glm::vec2(196, 196)));
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY));
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 0);
}
