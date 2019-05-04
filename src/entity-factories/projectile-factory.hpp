#pragma once

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "components/sprite.hpp"

class ProjectileFactory : public Factory {
public:
	ProjectileFactory(entt::DefaultRegistry& registry);
	virtual ~ProjectileFactory();

	void create(glm::vec2 initialPos, unsigned int targetId);

private:
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_projectileSprite;
};

