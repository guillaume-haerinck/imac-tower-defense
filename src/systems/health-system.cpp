#include "health-system.hpp"

#include <spdlog/spdlog.h>

#include "events/projectile-hit.hpp"

#include "components/health.hpp"

HealthSystem::HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: System(registry), m_emitter(emitter)
{
	m_emitter.on<evnt::ProjectileHit>([this](const evnt::ProjectileHit & event, EventEmitter & emitter) {
		m_registry.get<cmpt::Health>(event.targetId).current -= event.damage;
		if (m_registry.get<cmpt::Health>(event.targetId).current < 0) {
			m_registry.destroy(event.targetId);
		}
	});
}
