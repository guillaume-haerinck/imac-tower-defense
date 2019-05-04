#include "wave-system.hpp"

#include <spdlog/spdlog.h>

#include "events/start-wave.hpp"
#include "components/trajectory.hpp"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level)
: System(registry), m_emitter(emitter), m_enemyFactory(registry, level)
{
	m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		m_enemyFactory.create();
	});
}
