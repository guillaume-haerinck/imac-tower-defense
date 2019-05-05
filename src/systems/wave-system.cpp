#include "wave-system.hpp"

#include <spdlog/spdlog.h>
#include "components/trajectory.hpp"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level)
: ISystem(registry), m_emitter(emitter), m_enemyFactory(registry, level) {}

void WaveSystem::connectEvents() {
	auto connection = m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		// TODO handle the number of enemies from the number in the event
		this->m_enemyFactory.create();
	});
	m_waveCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::StartWave>>(connection);
}

void WaveSystem::disconnectEvents() {
	m_emitter.erase(*m_waveCon);
	m_waveCon.reset();
}

void WaveSystem::update(float deltatime) {

}
