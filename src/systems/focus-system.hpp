#pragma once

#include "system.hpp"
#include "entity-factories/projectile-factory.hpp"

class FocusSystem : public System {
public:
	FocusSystem(entt::DefaultRegistry& registry);
	void update();
private:
	ProjectileFactory m_projectileFactory;
};
