#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"

class HealthSystem : public System {
public:
	HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);

private:
	EventEmitter& m_emitter;
};
