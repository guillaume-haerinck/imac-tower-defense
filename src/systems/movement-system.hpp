#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"

class MovementSystem : public ISystem {
public:
	MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(float deltatime) override;
	void connectEvents() override;
	void disconnectEvents() override;

private:
	EventEmitter& m_emitter;
	glm::vec2 m_mousePos;
};
