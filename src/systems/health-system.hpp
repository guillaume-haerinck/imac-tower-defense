#pragma once

#include <entt/entt.hpp>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "entity-factories/explosion-factory.hpp"

class HealthSystem : public System {
public:
	HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);

private:
	EventEmitter& m_emitter;
	ExplosionFactory m_explosionFactory;
};
