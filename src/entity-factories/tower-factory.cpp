#include "tower-factory.hpp"

#include <glm/glm.hpp>

#include "core/tags.hpp"
#include "components/transform.hpp"
#include "components/look-at.hpp"
#include "components/shoot-at.hpp"
#include "components/rigid-body.hpp"
#include "components/targeting.hpp"
#include "components/trigger.hpp"
#include "services/locator.hpp"
#include "services/random/i-random.hpp"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry) : Factory(registry)
{
	m_towerSprite = m_spriteFactory.createSingle("res/images/textures/arrow.png", glm::vec2(8.0f));
}

void TowerFactory::create(float posX, float posY) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();

	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_towerSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY));
	m_registry.assign<cmpt::LookAt>(myEntity);
	m_registry.assign<cmpt::ShootAt>(myEntity, randomService.randInt(20, 60));
	m_registry.assign<cmpt::Targeting>(myEntity, -1);
	m_registry.assign<cmpt::Trigger>(myEntity, 25.0f);
}
