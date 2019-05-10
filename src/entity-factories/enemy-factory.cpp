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

// TODO doc ENTT partie "prototype" pour avoir des entity factory plus optimis�s en m�moire

EnemyFactory::EnemyFactory(entt::DefaultRegistry& registry, Level& level)
: Factory(registry), m_level(level)
{
	//m_ennemySprite = m_spriteFactory.createAtlas("res/images/spritesheets/spaceman-196x196.png", glm::vec2(13.0f), glm::vec2(196, 196));
	m_ennemySprite = m_spriteFactory.createSingle("res/images/textures/drone.png", glm::vec2(13.0f));
	m_healthBackground = m_primitiveFactory.createRect(glm::vec4(0, 0, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
	m_healthBar = m_primitiveFactory.createRect(glm::vec4(0, 1, 0, 1), glm::vec2(6.0f, 1.0f), PivotPoint::MIDDLE_LEFT);
}

EnemyFactory::~EnemyFactory() {
	GLCall(glDeleteTextures(1, &m_ennemySprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_ennemySprite.vaID));
	GLCall(glDeleteVertexArrays(1, &m_healthBackground.vaID));
	GLCall(glDeleteVertexArrays(1, &m_healthBar.vaID));
}

/* Old thing
void EnemyFactory::create(std::vector<glm::vec2> traj) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_ennemySprite);
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(traj.at(0).x, traj.at(0).y));
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 4);
	m_registry.assign<cmpt::Trajectory>(myEntity, traj);
}
*/

void EnemyFactory::create() {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	int startNode = m_level.getGraph()->getStartNodeRandom();

	auto myEntity = m_registry.create();
	m_registry.assign<entityTag::Enemy>(myEntity);
	m_registry.assign<cmpt::Sprite>(myEntity, m_ennemySprite);
	m_registry.assign<renderTag::Single>(myEntity);
	cmpt::Transform transform(m_level.getNodePosition(startNode), Z_INDEX_ENEMY);
	m_registry.assign<cmpt::Transform>(myEntity, transform);
	//m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 5);
	m_registry.assign<cmpt::Pathfinding>(myEntity, &m_level, startNode);
	m_registry.assign<cmpt::Health>(myEntity, ENNEMY_HEALTH);
	m_registry.assign<cmpt::HealthBar>(myEntity, glm::vec2(-3.0f, -7.0f), m_healthBackground, m_healthBar);
	m_registry.assign<cmpt::Hitbox>(myEntity, 5.0f);
}
