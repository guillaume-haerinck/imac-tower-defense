#include "tower-factory.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/look-at.hpp"
#include "components/shoot-at.hpp"
#include "components/rigid-body.hpp"
#include "components/targeting.hpp"
#include "components/hitbox.hpp"
#include "components/shoot-laser.hpp"
#include "components/health-bar.hpp"
#include "components/health.hpp"
#include "services/locator.hpp"
#include "services/random/i-random.hpp"

#include "components/constrained-rotation.hpp"

TowerFactory::TowerFactory(entt::DefaultRegistry& registry) : Factory(registry)
{
	m_towerSprite = m_spriteFactory.createSingle("res/images/textures/tower.png", glm::vec2(TOWER_HITBOX_RADIUS*2));
	m_healthBackground = m_primitiveFactory.createRect(glm::vec4(0, 0, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
	m_healthBar = m_primitiveFactory.createRect(glm::vec4(0, 1, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
}

TowerFactory::~TowerFactory() {
	GLCall(glDeleteTextures(1, &m_towerSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_towerSprite.vaID));
}

unsigned int TowerFactory::create(float posX, float posY) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();

	auto myEntity = m_registry.create();
	m_registry.assign<entityTag::Tower>(myEntity);
	m_registry.assign<cmpt::Sprite>(myEntity, m_towerSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY), zIndexTower);
	//m_registry.assign<cmpt::LookAt>(myEntity);
	//m_registry.assign<cmpt::ShootAt>(myEntity, randomService.randInt(20, 60));
	//m_registry.assign<cmpt::Targeting>(myEntity, -1, TOWER_ATTACK_RANGE);
	m_registry.assign<cmpt::Hitbox>(myEntity, TOWER_HITBOX_RADIUS);
	m_registry.assign<cmpt::ShootLaser>(myEntity);
	m_registry.assign<cmpt::Health>(myEntity, TOWER_HEALTH);
	m_registry.assign<cmpt::HealthBar>(myEntity, glm::vec2(-3.0f, -7.0f), m_healthBackground, m_healthBar);
	m_registry.assign<cmpt::ConstrainedRotation>(myEntity, 4);
	return myEntity;
}
