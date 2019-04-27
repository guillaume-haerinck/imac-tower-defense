#include "tower-factory.hpp"

#include <glm/glm.hpp>

#include "core/tags.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry)
: m_registry(registry)
{}

TowerFactory::~TowerFactory() {}

void TowerFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createSingle("res/images/textures/missing.png", glm::vec2(5.0f)));
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY));
}

