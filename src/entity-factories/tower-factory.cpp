#include "tower-factory.hpp"

#include <glm/glm.hpp>

#include "core/tags.hpp"
#include "components/sprite.hpp"
#include "components/transform-tile.hpp"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry)
: m_registry(registry)
{}

TowerFactory::~TowerFactory() {}

// TODO handle transformTile in render

void TowerFactory::create(int tileX, int tileY) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createSingle("res/images/textures/arrow.png", glm::vec2(1.0f)));
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::TransformTile>(myEntity, glm::vec2(tileX, tileY));
}

