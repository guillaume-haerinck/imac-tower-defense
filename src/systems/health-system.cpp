#include "health-system.hpp"

#include <spdlog/spdlog.h>

#include "events/enemy-dead.hpp"
#include "events/tower-dead.hpp"
#include "components/health.hpp"
#include "components/transform.hpp"
#include "components/shake.hpp"
#include "core/tags.hpp"
#include "core/constants.hpp"

HealthSystem::HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression)
: ISystem(registry, emitter), m_progression(progression)
{
	m_emitter.on<evnt::EnemyDamaged>([this](const evnt::EnemyDamaged & event, EventEmitter & emitter) {
		//Reduce health
		cmpt::Health& health = m_registry.get<cmpt::Health>(event.targetId);
		health.current -= event.damage;
		if (health.current <= 0.0001f) {
			if (m_registry.has<entityTag::Enemy>(event.targetId)) {
				m_emitter.publish<evnt::EnnemyDead>(event.position);
				m_progression.addToMoney(ENNEMY_MONEY_VALUE);
			}
			if (m_registry.has<entityTag::Tower>(event.targetId)) {
				m_emitter.publish<evnt::TowerDead>(m_registry.get<cmpt::Transform>(event.targetId).position);
			}
			m_registry.destroy(event.targetId);
		}
	});
}

void HealthSystem::update(float deltatime) {

}
