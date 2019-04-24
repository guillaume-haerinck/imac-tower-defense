#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "component-factories/primitive-factory.hpp"

class TileFactory
{
public:
	TileFactory(entt::DefaultRegistry& registry);
	~TileFactory();

	unsigned int createSpawn(glm::vec2 position);
	unsigned int createArrival(glm::vec2 position);
	unsigned int createPath(glm::vec2 position);
	unsigned int createConstructible(glm::vec2 position);
	unsigned int createLocked(glm::vec2 position);

private:
	entt::DefaultRegistry& m_registry;
	PrimitiveFactory m_primitiveFactory;
};

