#include "projectile-factory.hpp"

#include <glad/glad.h>

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/targeting.hpp"
#include "components/velocity.hpp"

#include "spdlog/spdlog.h"

ProjectileFactory::ProjectileFactory(entt::DefaultRegistry& registry) : Factory(registry) {
	m_projectileSprite = m_spriteFactory.createSingle("res/images/textures/projectile_snowflake.png", glm::vec2(6.0f));
}

ProjectileFactory::~ProjectileFactory() {
	GLCall(glDeleteTextures(1, &m_projectileSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_projectileSprite.vaID));
}

unsigned int ProjectileFactory::create(glm::vec2 initialPos, unsigned int targetId) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_projectileSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, initialPos, Z_INDEX_VISUAL_EFFECTS);
	m_registry.assign<targetingTag::Follow>(myEntity);
	m_registry.assign<cmpt::Targeting>(myEntity, targetId, PROJECTILE_HITBOX_RADIUS);
	m_registry.assign<cmpt::Velocity>(myEntity,SLOW_PROJECTILE_VELOCITY);
	return myEntity;
}

void ProjectileFactory::createSlow(glm::vec2 initialPos, unsigned int targetId) {
	unsigned int myEntity = create(initialPos, targetId);
	m_registry.assign<projectileType::Slow>(myEntity);
}

void ProjectileFactory::createDamage(glm::vec2 initialPos, unsigned int targetId) {
	unsigned int myEntity = create(initialPos, targetId);
	m_registry.assign<projectileType::Damage>(myEntity);
}
