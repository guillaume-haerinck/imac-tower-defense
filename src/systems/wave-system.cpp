#include "wave-system.hpp"

#include <spdlog/spdlog.h>
#include "events/wave-updated.hpp"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level)
: ISystem(registry, emitter), m_enemyFactory(registry, level), m_waveState(WaveState::NO),
  m_frameCount(0), m_secondsUntilWaveStart(0), m_timeUntilNextSpawn(0)
{
	m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		this->m_waveState = WaveState::PENDING;
		this->m_nbEnemyRemaingToSpawn = event.nbEnemyToSpawn;
		this->m_secondsUntilWaveStart = 0;
	});
}

void WaveSystem::update(float deltatime) {
	switch (m_waveState) {
	case WaveState::NO:
		break;

	case WaveState::PENDING:
		m_frameCount++;
		if (m_frameCount >= FRAMERATE) {
			m_secondsUntilWaveStart--;
			m_frameCount = 0;
			m_emitter.publish<evnt::WaveUpdated>(m_secondsUntilWaveStart, m_waveState);
		}
		if (m_secondsUntilWaveStart <= 0) {
			m_waveState = WaveState::DURING;
		}
		break;

	case WaveState::DURING:
		m_timeUntilNextSpawn--;
		if (m_timeUntilNextSpawn <= 0) {
			m_enemyFactory.createRobot();
			m_nbEnemyRemaingToSpawn--;
			m_timeUntilNextSpawn = FRAMERATE / 2;
		}
		if (m_nbEnemyRemaingToSpawn <= 0) {
			m_waveState = WaveState::NO;
			m_emitter.publish<evnt::WaveUpdated>(0, m_waveState);
		}
		break;

	default:
		break;
	}
}
