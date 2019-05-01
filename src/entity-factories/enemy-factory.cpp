#include "enemy-factory.hpp"

#include "core/tags.hpp"
#include "core/constants.hpp"
#include "components/transform.hpp"
#include "components/sprite-animation.hpp"
#include "components/trajectory.hpp"
#include "components/pathfinding.hpp"
#include "components/targeting.hpp"
#include "components/shoot-at.hpp"
#include "components/health.hpp"
#include "services/locator.hpp"
#include "services/random/i-random.hpp"

EnemyFactory::EnemyFactory(entt::DefaultRegistry& registry, Map& map)
: Factory(registry), m_map(map)
{
	m_ennemySprite = m_spriteFactory.createAtlas("res/images/spritesheets/spaceman-196x196.png", glm::vec2(13.0f), glm::vec2(196, 196));
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
	int startNode = m_map.m_graph.getStartNode();

	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_ennemySprite);
	m_registry.assign<renderTag::Atlas>(myEntity);
	cmpt::Transform transform(m_map.getNodePosition(startNode));
	m_registry.assign<cmpt::Transform>(myEntity, transform);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 25, 5);
	m_registry.assign<cmpt::Pathfinding>(myEntity, &m_map, startNode);
	m_registry.assign<cmpt::Health>(myEntity, 5);

	//Temporary : all towers have a chance to pick focus on the latest enemy created
	m_registry.view<cmpt::Targeting, cmpt::ShootAt>().each([myEntity, &randomService](auto entity, cmpt::Targeting & targeting , cmpt::ShootAt & shootAt) {
		if (randomService.random() < 0.25) {
			targeting.targetId = myEntity;
		}
	});
}
