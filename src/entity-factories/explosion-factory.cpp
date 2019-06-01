#include "explosion-factory.hpp"

#include <glad/glad.h>

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/sprite-animation.hpp"
#include "components/direction.hpp"
#include "components/velocity.hpp"
#include "components/age.hpp"
#include "components/animated.hpp"
#include "components/animation-alpha.hpp"
#include "components/tint-colour.hpp"


ExplosionFactory::ExplosionFactory(entt::DefaultRegistry& registry)
: Factory(registry)
{
	m_towerExplosionSprite = m_spriteFactory.createAtlas("res/images/spritesheets/explosion0-100x100.png", glm::vec2(30.0f), glm::vec2(100, 100),TOWER_EXPLOSION);
	m_enemyExplosionSprite = m_spriteFactory.createAtlas("res/images/spritesheets/explosion0-100x100.png", glm::vec2(30.0f), glm::vec2(100, 100), ENEMY_EXPLOSION);
	m_laserParticleSprite = m_spriteFactory.createSingle("res/images/textures/etincelle.png", glm::vec2(5.0f));
}

ExplosionFactory::~ExplosionFactory() {
	GLCall(glDeleteTextures(1, &m_towerExplosionSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_towerExplosionSprite.vaID));

	GLCall(glDeleteTextures(1, &m_enemyExplosionSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_enemyExplosionSprite.vaID));
}

void ExplosionFactory::create(glm::vec2 pos, ShaderType type = ENEMY_EXPLOSION ) {
	auto myEntity = m_registry.create();
	if (type == ENEMY_EXPLOSION) {
		m_registry.assign<cmpt::Sprite>(myEntity, m_enemyExplosionSprite);
	}
	else {
		m_registry.assign<cmpt::Sprite>(myEntity, m_towerExplosionSprite);
	}
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<renderTag::OneTimeAtlas>(myEntity);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 99, 2);
	m_registry.assign<cmpt::Transform>(myEntity, pos, Z_INDEX_VISUAL_EFFECTS);
}

void ExplosionFactory::createLaserParticle(glm::vec2 pos, float dirAgl) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_laserParticleSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, pos, Z_INDEX_VISUAL_EFFECTS);
	m_registry.assign<cmpt::Direction>(myEntity, dirAgl);
	m_registry.assign<cmpt::Velocity>(myEntity, 60.0f);
	m_registry.assign<cmpt::Animated>(myEntity, 0.1f,true);
	//m_registry.assign<cmpt::AnimationAlpha>(myEntity, false);
	m_registry.assign<renderOrderTag::o_VFX>(myEntity);
}