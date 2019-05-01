#include "health-system.hpp"

#include <spdlog/spdlog.h>

#include "events/projectile-hit.hpp"
#include "events/enemy-dead.hpp"

#include "components/health.hpp"
#include "components/transform.hpp"

HealthSystem::HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
	: System(registry), m_emitter(emitter)
{
	m_emitter.on<evnt::ProjectileHit>([this](const evnt::ProjectileHit & event, EventEmitter & emitter) {
		cmpt::Health& health = m_registry.get<cmpt::Health>(event.targetId);
		health.current -= event.damage;
		if (health.current <= 0.2f) {
			m_emitter.publish<evnt::EnnemyDead>(event.position);
			m_registry.destroy(event.targetId);
		}
	});
}
