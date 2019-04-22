#pragma once

#include <entt/entt.hpp>

#include "component-factories/sprite-factory.hpp"

class TowerFactory {
public:
	TowerFactory(entt::DefaultRegistry& registry);
	~TowerFactory();

	void create(int tileX, int tileY);

private:
	entt::DefaultRegistry& m_registry;
	SpriteFactory m_spriteFactory;
};
