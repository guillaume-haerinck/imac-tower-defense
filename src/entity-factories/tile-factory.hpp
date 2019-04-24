#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "component-factories/primitive-factory.hpp"

class TileFactory
{
public:
	TileFactory(entt::DefaultRegistry& registry);
	~TileFactory();

	void create(glm::vec2 position, glm::vec4 color);

private:
	entt::DefaultRegistry& m_registry;
	PrimitiveFactory m_primitiveFactory;
};

