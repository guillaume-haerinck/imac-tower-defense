#pragma once

#include <entt/entt.hpp>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/start-wave.hpp"
#include "core/level/level.hpp"
#include "entity-factories/enemy-factory.hpp"
#include "core/progression.hpp"

enum class WaveState {
	NOT_STARTED,
	PENDING,
	DURING,
	DONE
};

class WaveSystem : public ISystem {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression, Level& level);
	void update(float deltatime) override;

private:
	Progression & m_progression;
	EnemyFactory m_enemyFactory;
	WaveState m_waveState;
	unsigned int m_frameCount;
	int m_timer;
	int m_nbRobotRemaingToSpawn;
	int m_nbKamikazeRemaingToSpawn;
	float m_probaSpawnKamikaze;
};
