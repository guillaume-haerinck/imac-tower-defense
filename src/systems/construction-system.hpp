#pragma once

#include <entt/entt.hpp>

#include "system.hpp"
#include "events/event-emitter.hpp"

class ConstructionSystem : public System {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update();

private:
	EventEmitter& m_emitter;
};
