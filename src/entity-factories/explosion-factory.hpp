#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "components/sprite.hpp"

class ExplosionFactory : public Factory {
public:
	ExplosionFactory(entt::DefaultRegistry& registry);
	virtual ~ExplosionFactory();

	void create(glm::vec2 pos);

private:
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_explosionSprite;
};

