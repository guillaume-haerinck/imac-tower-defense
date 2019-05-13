#pragma once

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "components/sprite.hpp"

enum ProjectileType {
	SLOW,
	DAMAGE
};

class ProjectileFactory : public Factory {
public:
	ProjectileFactory(entt::DefaultRegistry& registry);
	virtual ~ProjectileFactory();

	void createSlow(glm::vec2 initialPos, unsigned int targetId);
	void createDamage(glm::vec2 initialPos, unsigned int targetId);

private:
	unsigned int create(glm::vec2 initialPos, unsigned int targetId);
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_projectileSprite;
};

