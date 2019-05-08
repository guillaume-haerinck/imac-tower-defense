#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"

class MovementSystem : public ISystem {
public:
	MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(float deltatime) override;
	void onMouseMove(const evnt::MouseMove& event) override;

private:
	glm::vec2 m_mousePos;
	glm::vec2 m_prevMousePos;
};
