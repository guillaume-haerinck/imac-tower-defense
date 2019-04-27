#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"

class TowerFactory : public Factory {
public:
	TowerFactory(entt::DefaultRegistry& registry);

	void create(float posX, float posY);

private:
	SpriteFactory m_spriteFactory;
};
