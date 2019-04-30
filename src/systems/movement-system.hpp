#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"

class MovementSystem : public System {
public:
	MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(double deltatime);

private:
	EventEmitter& m_emitter;
	glm::vec2 m_mousePos;
};
