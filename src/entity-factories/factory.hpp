#pragma once

#include <entt/entt.hpp>

class Factory {
protected:
	Factory(entt::DefaultRegistry& registry) : m_registry(registry) {}
	entt::DefaultRegistry& m_registry;
};
