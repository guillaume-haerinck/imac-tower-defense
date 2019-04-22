#pragma once

#include <entt/entt.hpp>

#include "component-factories/sprite-factory.hpp"

class EnemyFactory {
public:
	EnemyFactory(entt::DefaultRegistry& registry);
	~EnemyFactory();

	void create(float posX, float posY);

private:
	entt::DefaultRegistry& m_registry;
	SpriteFactory m_spriteFactory;
};

