#include "tile-factory.hpp"

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/primitive.hpp"

TileFactory::TileFactory(entt::DefaultRegistry& registry) : Factory(registry) {
	m_spawn = m_primitiveFactory.createRect(glm::vec4(0, 1, 0, 1), glm::vec2(TILE_SIZE));
	m_arrival = m_primitiveFactory.createRect(glm::vec4(1, 0, 0, 1), glm::vec2(TILE_SIZE));
	m_path = m_primitiveFactory.createRect(glm::vec4(1, 1, 1, 1), glm::vec2(TILE_SIZE));
	m_constructible = m_primitiveFactory.createRect(glm::vec4(0, 0, 1, 1), glm::vec2(TILE_SIZE));
	m_locked = m_primitiveFactory.createRect(glm::vec4(0.5, 0.5, 0.5, 1), glm::vec2(TILE_SIZE));
}

TileFactory::~TileFactory() {
	GLCall(glDeleteVertexArrays(1, &m_spawn.vaID));
	GLCall(glDeleteVertexArrays(1, &m_arrival.vaID));
	GLCall(glDeleteVertexArrays(1, &m_path.vaID));
	GLCall(glDeleteVertexArrays(1, &m_constructible.vaID));
	GLCall(glDeleteVertexArrays(1, &m_locked.vaID));
}

unsigned int TileFactory::createSpawn(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_spawn);
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createArrival(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_arrival);
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createPath(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_path);
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}

unsigned int TileFactory::createConstructible(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_constructible);
	m_registry.assign<cmpt::Transform>(myEntity, position);
	m_registry.assign<tileTag::Constructible>(myEntity);
	return myEntity;
}

unsigned int TileFactory::createLocked(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Primitive>(myEntity, m_locked);
	m_registry.assign<cmpt::Transform>(myEntity, position);
	return myEntity;
}
