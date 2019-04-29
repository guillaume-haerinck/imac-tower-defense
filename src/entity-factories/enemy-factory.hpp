#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "entity-factories/projectile-factory.hpp"

#include "map/map.hpp"

class EnemyFactory : public Factory {
public:
	EnemyFactory(entt::DefaultRegistry& registry, Map& map);

	void create(std::vector<glm::vec2> traj);
	void create();

public:
	SpriteFactory m_spriteFactory;
	Map& m_map;
};

