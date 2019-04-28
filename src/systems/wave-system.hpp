#pragma once

#include "system.hpp"
#include "events/event-emitter.hpp"
#include "map/map.hpp"
#include "entity-factories/enemy-factory.hpp"

class WaveSystem : public System {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map);

private:
	EventEmitter& m_emitter;
	Map& m_map;
	EnemyFactory m_enemyFactory;
};
