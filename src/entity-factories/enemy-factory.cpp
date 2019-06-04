#include "enemy-factory.hpp"

#include <glad/glad.h>

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/sprite-animation.hpp"
#include "components/trajectory.hpp"
#include "components/pathfinding.hpp"
#include "components/targeting.hpp"
#include "components/shoot-at.hpp"
#include "components/hitbox.hpp"
#include "components/health-bar.hpp"
#include "components/health.hpp"
#include "services/locator.hpp"
#include "services/random/i-random.hpp"
#include "components/wiggle.hpp"
#include "components/attached-to.hpp"
#include "components/look-at-mouse.hpp"
#include "components/move-towards-mouse.hpp"
#include "components/velocity.hpp"
#include "components/shake.hpp"
#include "components/tint-colour.hpp"
#include "components/animated.hpp"
#include "components/animation-scale.hpp"
#include "components/animation-dark.hpp"
#include "components/animation-alpha.hpp"

// TODO doc ENTT partie "prototype" pour avoir des entity factory plus optimis�s en m�moire

EnemyFactory::EnemyFactory(entt::DefaultRegistry& registry, Level& level)
: Factory(registry), m_level(level)
{
	m_robotSprite = m_spriteFactory.createAtlas("res/images/spritesheets/enemy-small-robot-77x117.png", glm::vec2(TILE_SIZE * 0.66, TILE_SIZE), glm::vec2(77, 117));
	m_droneSprite = m_spriteFactory.createSingle("res/images/textures/drone-no-eye.png", glm::vec2(13.0f));
	m_droneEyeSprite = m_spriteFactory.createSingle("res/images/textures/drone-eye.png", glm::vec2(13.0f));
	m_healthBackground = m_primitiveFactory.createRect(glm::vec4(0, 0, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
	m_healthBar = m_primitiveFactory.createRect(glm::vec4(0, 1, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
}

EnemyFactory::~EnemyFactory() {
	GLCall(glDeleteTextures(1, &m_droneSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_droneSprite.vaID));
	GLCall(glDeleteTextures(1, &m_droneEyeSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_droneEyeSprite.vaID));
	GLCall(glDeleteTextures(1, &m_robotSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_robotSprite.vaID));
	GLCall(glDeleteVertexArrays(1, &m_healthBackground.vaID));
	GLCall(glDeleteVertexArrays(1, &m_healthBar.vaID));
}

void EnemyFactory::createRobot() {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	std::uint32_t entity = create();
	//m_registry.assign<enemyTag::Robot>(entity);
	m_registry.assign<enemyTag::Kamikaze>(entity);
	m_registry.assign<cmpt::Health>(entity, ENEMY_DRONE_HEALTH + randomService.random(-ENEMY_HEALTH_RANDOM_VARIATION, ENEMY_HEALTH_RANDOM_VARIATION));
	m_registry.assign<cmpt::Sprite>(entity, m_robotSprite);
	m_registry.assign<renderTag::Atlas>(entity);
	m_registry.assign<cmpt::SpriteAnimation>(entity, 0, 11, 0.5);
}

void EnemyFactory::createKamikaze() {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	std::uint32_t entity = create();
	m_registry.assign<cmpt::Sprite>(entity, m_droneSprite);
	m_registry.assign<renderTag::Single>(entity);
	m_registry.assign<enemyTag::Kamikaze>(entity);
	m_registry.assign<cmpt::Health>(entity, ENEMY_KAMIKAZE_HEALTH + randomService.random(-ENEMY_HEALTH_RANDOM_VARIATION, ENEMY_HEALTH_RANDOM_VARIATION));
	m_registry.assign<cmpt::TintColour>(entity, glm::vec4(1, 0, 0, 0.5));

	std::uint32_t eye = m_registry.create();
	m_registry.assign<cmpt::Transform>(eye, glm::vec2(0), Z_INDEX_ENEMY + 1);
	m_registry.assign<cmpt::AttachedTo>(eye, entity);
	m_registry.assign<cmpt::Sprite>(eye, m_droneEyeSprite);
	m_registry.assign<renderTag::Single>(eye);
	m_registry.assign<cmpt::MoveTowardsMouse>(eye, 0.8);
	m_registry.assign<renderOrderTag::o_Enemy2>(eye);
	//m_registry.assign<cmpt::Wiggle>(eye,0.7);
}


/* --------- Private methods ---------- */

std::uint32_t EnemyFactory::create() {
	int startNode = m_level.getGraph()->getStartNodeRandom();

	std::uint32_t myEntity = m_registry.create();
	m_registry.assign<entityTag::Enemy>(myEntity);
	cmpt::Transform transform(m_level.getNodePosition(startNode), Z_INDEX_ENEMY);
	m_registry.assign<cmpt::Transform>(myEntity, transform);
	//m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 5);
	m_registry.assign<cmpt::Pathfinding>(myEntity, &m_level, startNode);
	m_registry.assign<cmpt::HealthBar>(myEntity, glm::vec2(-3.0f, -7.0f), m_healthBackground, m_healthBar);
	m_registry.assign<cmpt::Hitbox>(myEntity, 5.0f);
	m_registry.assign<cmpt::Wiggle>(myEntity, 1);
	m_registry.assign<cmpt::Velocity>(myEntity, ENEMY_VELOCITY);
	m_registry.assign<cmpt::Shake>(myEntity);
	//m_registry.assign<cmpt::TintColour>(myEntity, glm::vec4(randomService.random(), randomService.random(), randomService.random(), 0.5));
	m_registry.assign<cmpt::Animated>(myEntity, 2, false);
	m_registry.assign<cmpt::AnimationScale>(myEntity, true);
	m_registry.assign<cmpt::AnimationDark>(myEntity, true);
	m_registry.assign<cmpt::AnimationAlpha>(myEntity, true);
	m_registry.assign<renderOrderTag::o_Enemy>(myEntity);

	return myEntity;
}
