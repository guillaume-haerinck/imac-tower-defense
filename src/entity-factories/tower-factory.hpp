#pragma once

#include <entt/entt.hpp>

#include "component-factories/sprite-factory.hpp"

class TowerFactory {
public:
	TowerFactory(entt::DefaultRegistry& registry);
	~TowerFactory();

	void create(float posX, float posY);

private:
	entt::DefaultRegistry& m_registry;
	SpriteFactory m_spriteFactory;
};
