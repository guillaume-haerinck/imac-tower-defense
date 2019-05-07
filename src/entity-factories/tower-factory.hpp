#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "component-factories/primitive-factory.hpp"
#include "components/sprite.hpp"
#include "components/primitive.hpp"

class TowerFactory : public Factory {
public:
	TowerFactory(entt::DefaultRegistry& registry);
	virtual ~TowerFactory();

	void create(float posX, float posY);

private:
	SpriteFactory m_spriteFactory;
	PrimitiveFactory m_primitiveFactory;
	cmpt::Sprite m_towerSprite;
	cmpt::Primitive m_healthBackground;
	cmpt::Primitive m_healthBar;
};
