#pragma once

#include <entt/entt.hpp>
#include <Box2D/Box2D.h>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "component-factories/rigid-body-factory.hpp"
#include "entity-factories/projectile-factory.hpp"
#include "components/sprite.hpp"

#include "map/map.hpp"

class EnemyFactory : public Factory {
public:
	EnemyFactory(entt::DefaultRegistry& registry, Map& map, b2World& physicWorld);

	void create(std::vector<glm::vec2> traj);
	void create();

public:
	SpriteFactory m_spriteFactory;
	RigidBodyFactory m_rigidBodyFactory;
	cmpt::Sprite m_ennemySprite;
	Map& m_map;
};

