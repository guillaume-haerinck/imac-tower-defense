#include "wave-system.hpp"

#include <spdlog/spdlog.h>

#include "events/start-wave.hpp"
#include "components/trajectory.hpp"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map, b2World& physicWorld)
: System(registry), m_emitter(emitter), m_map(map), m_enemyFactory(registry, map, physicWorld)
{
	m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		m_enemyFactory.create();
	});
}
