#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "core/map/map.hpp"
#include "entity-factories/enemy-factory.hpp"

class WaveSystem : public System {
public:
	WaveSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map);

private:
	EventEmitter& m_emitter;
	EnemyFactory m_enemyFactory;
};
