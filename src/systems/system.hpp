#pragma once

#include <entt/entt.hpp>

class System {
protected:
	System(entt::DefaultRegistry& registry) : m_registry(registry) {}
	entt::DefaultRegistry& m_registry;
};
