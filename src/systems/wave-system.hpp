#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "map/map.hpp"
#include "entity-factories/enemy-factory.hpp"

class WaveSystem : public System {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map, b2World& physicWorld);

private:
	EventEmitter& m_emitter;
	Map& m_map;
	EnemyFactory m_enemyFactory;
};
