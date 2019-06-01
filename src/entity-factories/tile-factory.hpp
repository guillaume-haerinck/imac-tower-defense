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

	std::uint32_t createSpawn(glm::vec2 position);
	std::uint32_t createArrival(glm::vec2 position);
	std::uint32_t createPath(glm::vec2 position);
	std::uint32_t createConstructible(glm::vec2 position);
	std::uint32_t createLocked(glm::vec2 position);

private:
	SpriteFactory m_spriteFactory;
	PrimitiveFactory m_primitiveFactory;

	std::uint32_t m_create(glm::vec2 position);

	cmpt::Sprite m_spawn;
	cmpt::Sprite m_arrival;
	cmpt::Sprite m_path;
	cmpt::Primitive m_locked;
};

