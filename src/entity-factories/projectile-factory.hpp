#pragma once

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"

class ProjectileFactory : public Factory {
public:
	ProjectileFactory(entt::DefaultRegistry& registry);

	void create(glm::vec2 initialPos, unsigned int targetId);

private:
	SpriteFactory m_spriteFactory;
};

