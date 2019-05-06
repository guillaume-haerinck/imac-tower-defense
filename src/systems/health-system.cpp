#include "health-system.hpp"

#include <spdlog/spdlog.h>

#include "events/enemy-dead.hpp"
#include "components/health.hpp"
#include "components/transform.hpp"

HealthSystem::HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression)
: ISystem(registry), m_emitter(emitter), m_progression(progression) {}

void HealthSystem::connectEvents() {
	if (m_bConnected == false) {
		auto connection = m_emitter.on<evnt::EnemyDamaged>([this](const evnt::EnemyDamaged & event, EventEmitter & emitter) {
			cmpt::Health& health = m_registry.get<cmpt::Health>(event.targetId);
			health.current -= event.damage;
			if (health.current < 0 ) {
				m_emitter.publish<evnt::EnnemyDead>(event.position);
				m_registry.destroy(event.targetId);
				m_progression.addToMoney(2);
			}
			});
		m_damagedCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::EnemyDamaged>>(connection);

		m_bConnected = true;
	}
}

void HealthSystem::disconnectEvents() {
	if (m_bConnected == true) {
		m_emitter.erase(*m_damagedCon);
		m_damagedCon.reset();

		m_bConnected = false;
	}
}

void HealthSystem::update(float deltatime) {

}
