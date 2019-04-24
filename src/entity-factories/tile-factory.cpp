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

unsigned int TileFactory::createSpawn(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_primitiveFactory.createRect(glm::vec4(0, 1, 0, 1), glm::vec2(TILE_SIZE)));
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createArrival(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_primitiveFactory.createRect(glm::vec4(1, 0, 0, 1), glm::vec2(TILE_SIZE)));
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createPath(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_primitiveFactory.createRect(glm::vec4(1, 1, 1, 1), glm::vec2(TILE_SIZE)));
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createConstructible(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_primitiveFactory.createRect(glm::vec4(0, 0, 1, 1), glm::vec2(TILE_SIZE)));
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createLocked(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_primitiveFactory.createRect(glm::vec4(0.5, 0.5, 0.5, 1), glm::vec2(TILE_SIZE)));
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}
