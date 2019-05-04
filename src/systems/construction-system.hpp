#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"
#include "entity-factories/tower-factory.hpp"
#include "events/handlers/event-emitter.hpp"
#include "core/level/level.hpp"

class ConstructionSystem : public System {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level);

private:
	EventEmitter& m_emitter;
	Level& m_level;
	TowerFactory m_towerFactory;
};
