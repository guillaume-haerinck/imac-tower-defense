#include "projectile-factory.hpp"

#include "core/tags.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"
#include "components/follow.hpp"
#include "components/targeting.hpp"

ProjectileFactory::ProjectileFactory(entt::DefaultRegistry& registry) : Factory(registry) {}

void ProjectileFactory::create(glm::vec2 initialPos, unsigned int targetId) {
	//spdlog::info("{}", targetId);
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Sprite>(myEntity, m_spriteFactory.createSingle("res/images/textures/missing.png", glm::vec2(2.0f)));
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, initialPos);
	m_registry.assign<cmpt::Follow>(myEntity,2.5);
	m_registry.assign<cmpt::Targeting>(myEntity,targetId);
}
