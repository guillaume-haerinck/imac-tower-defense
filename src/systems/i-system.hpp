#pragma once

#include <entt/entt.hpp>

class ISystem {
public:
	virtual void connectEvents() = 0;
	virtual void disconnectEvents() = 0;
	virtual void update(float deltatime) = 0;

protected:
	ISystem(entt::DefaultRegistry& registry) : m_registry(registry) {}
	entt::DefaultRegistry& m_registry;
};
