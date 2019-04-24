#include "tile-factory.hpp"

#include "core/constants.hpp"
#include "components/transform.hpp"
#include "components/primitive.hpp"

TileFactory::TileFactory(entt::DefaultRegistry& registry)
: m_registry(registry)
{
}

TileFactory::~TileFactory() {
}

void TileFactory::create(glm::vec2 position, glm::vec4 color) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_primitiveFactory.createRect(color, glm::vec2(TILE_SIZE)));
	m_registry.assign<cmpt::Transform>(myEntity, position);
}
