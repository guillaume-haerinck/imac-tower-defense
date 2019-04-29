#pragma once

#include <entt/entt.hpp>

#include "system.hpp"
#include "entity-factories/tower-factory.hpp"
#include "events/event-emitter.hpp"
#include "map/map.hpp"

class ConstructionSystem : public System {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map, b2World& physicWorld);

private:
	EventEmitter& m_emitter;
	Map& m_map;
	TowerFactory m_towerFactory;
};