#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "events/handlers/event-emitter.hpp"
#include "events/handlers/input-handler.hpp"

class ISystem : public InputHandler {
public:
	virtual void update(float deltatime) = 0;

protected:
	ISystem(entt::DefaultRegistry& registry, EventEmitter& emitter) : InputHandler(emitter), m_registry(registry) {}

	entt::DefaultRegistry& m_registry;
};
