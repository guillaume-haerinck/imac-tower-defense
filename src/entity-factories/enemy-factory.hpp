#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "entity-factories/projectile-factory.hpp"
#include "components/sprite.hpp"
#include "components/primitive.hpp"

#include "core/level/level.hpp"

class EnemyFactory : public Factory {
public:
	EnemyFactory(entt::DefaultRegistry& registry, Level& level);
	virtual ~EnemyFactory();

	// void create(std::vector<glm::vec2> traj);
	void create();

private:
	SpriteFactory m_spriteFactory;
	PrimitiveFactory m_primitiveFactory;
	cmpt::Sprite m_ennemySprite;
	cmpt::Primitive m_healthBackground;
	cmpt::Primitive m_healthBar;
	Level& m_level;
};

