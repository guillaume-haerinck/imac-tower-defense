#include "wave-system.hpp"

#include <spdlog/spdlog.h>

#include "events/start-wave.hpp"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map)
: System(registry), m_emitter(emitter), m_map(map), m_enemyFactory(registry)
{
	m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		spdlog::info("Wave started !");
	});
}
