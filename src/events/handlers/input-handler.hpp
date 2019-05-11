#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "events/handlers/event-emitter.hpp"
#include "events/inputs/left-click-up.hpp"
#include "events/inputs/left-click-down.hpp"
#include "events/inputs/right-click-up.hpp"
#include "events/inputs/right-click-down.hpp"
#include "events/inputs/mouse-move.hpp"

class InputHandler {
public:
	// Subscription
	void connectInputs();
	void disconnectInputs();

	// Events
	virtual void onMouseMove(const evnt::MouseMove& event);

	virtual void onLeftClickUp(const evnt::LeftClickUp& event);
	virtual void onLeftClickDown(const evnt::LeftClickDown& event);

	virtual void onRightClickUp(const evnt::RightClickUp& event);
	virtual void onRightClickDown(const evnt::RightClickDown& event);

protected:
	InputHandler(EventEmitter& emitter) : m_emitter(emitter), m_bConnected(false) {}
	EventEmitter& m_emitter;

private:
	bool m_bConnected;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_leftClickUp;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>> m_leftClickDown;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>> m_rightClickUp;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickDown>> m_rightClickDown;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::MouseMove>> m_mouseMove;
};
