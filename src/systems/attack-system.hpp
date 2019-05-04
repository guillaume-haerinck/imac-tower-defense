#pragma once

#include "system.hpp"
#include "entity-factories/projectile-factory.hpp"

#include "components/trigger.hpp"
#include "components/transform.hpp"

class AttackSystem : public System {
public:
	AttackSystem(entt::DefaultRegistry& registry);
	void update();
private:
	ProjectileFactory m_projectileFactory;
	bool isInRange(cmpt::Transform transform1, cmpt::Trigger trigger1, cmpt::Transform transform2, cmpt::Trigger trigger2);
	bool isInRange(cmpt::Transform transform1, cmpt::Trigger trigger1, unsigned int targetId);
};
