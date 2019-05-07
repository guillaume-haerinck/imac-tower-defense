#pragma once

#include <entt/entt.hpp>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/start-wave.hpp"
#include "core/level/level.hpp"
#include "entity-factories/enemy-factory.hpp"

class WaveSystem : public ISystem {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& map);
	void update(float deltatime) override;

private:
	EnemyFactory m_enemyFactory;
};
