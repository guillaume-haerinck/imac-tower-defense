#include "tile-factory.hpp"

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/primitive.hpp"
#include "components/sprite-animation.hpp"

TileFactory::TileFactory(entt::DefaultRegistry& registry) : Factory(registry) {
	m_spawn = m_spriteFactory.createSingle("res/images/textures/spawn.png", glm::vec2(TILE_SIZE));
	m_arrival = m_spriteFactory.createAtlas("res/images/spritesheets/arrival_portal-200x200.png", glm::vec2(TILE_SIZE), glm::vec2(200, 200));
	m_path = m_spriteFactory.createSingle("res/images/textures/path.png", glm::vec2(TILE_SIZE));
	m_constructible = m_spriteFactory.createSingle("res/images/textures/constructible.png", glm::vec2(TILE_SIZE));
	m_locked = m_primitiveFactory.createRect(glm::vec4(0.5, 0.5, 0.5, 1), glm::vec2(TILE_SIZE));
}

TileFactory::~TileFactory() {
	GLCall(glDeleteVertexArrays(1, &m_spawn.vaID));
	GLCall(glDeleteVertexArrays(1, &m_arrival.vaID));
	GLCall(glDeleteVertexArrays(1, &m_path.vaID));
	GLCall(glDeleteVertexArrays(1, &m_constructible.vaID));
	GLCall(glDeleteVertexArrays(1, &m_locked.vaID));
}

std::uint32_t TileFactory::m_create(glm::vec2 position) {
	auto myEntity = m_registry.create();
	m_registry.assign<cmpt::Transform>(myEntity, position, Z_INDEX_MAP);
	m_registry.assign<entityTag::Tile>(myEntity);
	m_registry.assign<renderOrderTag::o_Tile>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createSpawn(glm::vec2 position) {
	std::uint32_t myEntity = m_create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_spawn);
	m_registry.assign<renderTag::Single>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createArrival(glm::vec2 position) {
	std::uint32_t myEntity = m_create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_arrival);
	m_registry.assign<renderTag::Atlas>(myEntity);
	m_registry.assign<cmpt::SpriteAnimation>(myEntity, 0, 99, 2);
	return myEntity;
}

std::uint32_t TileFactory::createPath(glm::vec2 position) {
	std::uint32_t myEntity = m_create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_path);
	m_registry.assign<renderTag::Single>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createConstructible(glm::vec2 position) {
	std::uint32_t myEntity = m_create(position);
	m_registry.assign<cmpt::Sprite>(myEntity, m_constructible);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<tileTag::Constructible>(myEntity);
	return myEntity;
}

std::uint32_t TileFactory::createLocked(glm::vec2 position) {
	std::uint32_t myEntity = m_create(position);
	m_registry.assign<cmpt::Primitive>(myEntity, m_locked);
	return myEntity;
}
