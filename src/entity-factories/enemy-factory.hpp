#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"

class EnemyFactory : public Factory {
public:
	EnemyFactory(entt::DefaultRegistry& registry);

	void create(std::vector<glm::vec2> traj);

private:
	SpriteFactory m_spriteFactory;
};

