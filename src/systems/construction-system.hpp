#pragma once

#include <entt/entt.hpp>

#include "system.hpp"

class ConstructionSystem : public System {
public:
	ConstructionSystem(entt::DefaultRegistry& registry);

	void update();
};
