#include "wave-system.hpp"

#include <spdlog/spdlog.h>

#include "services/locator.hpp"
#include "services/audio/i-audio.hpp"
#include "events/wave-updated.hpp"

WaveSystem::WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level)
: ISystem(registry, emitter), m_enemyFactory(registry, level), m_waveState(WaveState::NO),
  m_frameCount(0), m_timer(0), m_spawnRate(0)
{
	m_emitter.on<evnt::StartWave>([this](const evnt::StartWave & event, EventEmitter & emitter) {
		this->m_waveState = WaveState::PENDING;
		this->m_nbEnemyRemaingToSpawn = event.nbEnemyToSpawn;
		this->m_spawnRate = event.spawnRate;
		this->m_timer = 3; // Time for the animation
		IAudio& audioService = entt::ServiceLocator<IAudio>::ref();
		audioService.stopAllSounds();
		audioService.playSound(AudioFiles::MUSIC_WAVE);
		audioService.playSound(AudioFiles::EFFECT_CLICK);
	});
}

void WaveSystem::update(float deltatime) {
	switch (m_waveState) {
	case WaveState::NO:
		break;

	case WaveState::PENDING:
		m_frameCount++;
		if (m_frameCount >= FRAMERATE) {
			m_timer--;
			m_frameCount = 0;
			m_emitter.publish<evnt::WaveUpdated>(m_timer, m_waveState);
		}
		if (m_timer <= 0) {
			m_waveState = WaveState::DURING;
			m_timer = m_nbEnemyRemaingToSpawn * m_spawnRate;
		}
		break;

	case WaveState::DURING:
		m_frameCount++;
		if (m_frameCount >= FRAMERATE) {
			// Spawn one robot per second
			m_frameCount = 0;
			m_emitter.publish<evnt::WaveUpdated>(m_timer, m_waveState);
			m_enemyFactory.createRobot();
			m_timer--;
		}
		if (m_timer <= 0) {
			m_waveState = WaveState::NO;
			m_emitter.publish<evnt::WaveUpdated>(0, m_waveState);
		}
		break;

	default:
		break;
	}
}
