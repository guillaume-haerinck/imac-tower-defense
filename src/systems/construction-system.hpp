#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"
#include "entity-factories/tower-factory.hpp"
#include "events/handlers/event-emitter.hpp"
#include "map/map.hpp"

class ConstructionSystem : public System {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map);

private:
	EventEmitter& m_emitter;
	Map& m_map;
	TowerFactory m_towerFactory;
};
