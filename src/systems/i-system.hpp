#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "events/handlers/event-emitter.hpp"
#include "events/left-click-up.hpp"
#include "events/left-click-down.hpp"
#include "events/right-click-up.hpp"
#include "events/right-click-down.hpp"
#include "events/mouse-move.hpp"

class ISystem {
public:
	virtual void update(float deltatime) = 0;
	
	// Input events
	void connectInputs();
	void disconnectInputs();

	virtual void onMouseMove(const evnt::MouseMove& event);

	virtual void onLeftClickUp(const evnt::LeftClickUp& event);
	virtual void onLeftClickDown(const evnt::LeftClickDown& event);

	virtual void onRightClickUp(const evnt::RightClickUp& event);
	virtual void onRightClickDown(const evnt::RightClickDown& event);

protected:
	ISystem(entt::DefaultRegistry& registry, EventEmitter& emitter) : m_registry(registry), m_emitter(emitter), m_bConnected(false) {}

	entt::DefaultRegistry& m_registry;
	EventEmitter& m_emitter;
	bool m_bConnected;

private:
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_leftClickUp;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>> m_leftClickDown;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>> m_rightClickUp;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>> m_rightClickDown;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::MouseMove>> m_mouseMove;
};
