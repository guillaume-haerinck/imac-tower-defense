#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "components/sprite.hpp"

class TowerFactory : public Factory {
public:
	TowerFactory(entt::DefaultRegistry& registry);
	virtual ~TowerFactory();

	void create(float posX, float posY);

private:
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_towerSprite;
};
