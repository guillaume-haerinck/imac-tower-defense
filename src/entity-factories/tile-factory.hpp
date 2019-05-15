#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <glad/glad.h>

#include "factory.hpp"
#include "component-factories/primitive-factory.hpp"
#include "component-factories/sprite-factory.hpp"

class TileFactory : public Factory {
public:
	TileFactory(entt::DefaultRegistry& registry);
	virtual ~TileFactory();

	unsigned int createSpawn(glm::vec2 position);
	unsigned int createArrival(glm::vec2 position);
	unsigned int createPath(glm::vec2 position);
	unsigned int createConstructible(glm::vec2 position);
	unsigned int createLocked(glm::vec2 position);

private:
	SpriteFactory m_spriteFactory;
	PrimitiveFactory m_primitiveFactory;

	cmpt::Sprite m_spawn;
	cmpt::Sprite m_arrival;
	cmpt::Sprite m_path;
	cmpt::Sprite m_constructible;
	cmpt::Primitive m_locked;
};

