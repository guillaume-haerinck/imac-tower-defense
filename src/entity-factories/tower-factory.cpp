#include "tower-factory.hpp"

#include <glm/glm.hpp>

#include "core/tags.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"
#include "components/look-at.hpp"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry) : Factory(registry) {}

void TowerFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createSingle("res/images/textures/missing.png", glm::vec2(5.0f)));
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY));
	m_registry.assign<cmpt::LookAt>(myEntity, 0);
}
