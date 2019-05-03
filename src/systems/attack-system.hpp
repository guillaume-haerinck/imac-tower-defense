#pragma once

#include "system.hpp"
#include "entity-factories/projectile-factory.hpp"

class AttackSystem : public System {
public:
	AttackSystem(entt::DefaultRegistry& registry);
	void update();
private:
	ProjectileFactory m_projectileFactory;
};
