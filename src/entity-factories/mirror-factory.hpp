#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "components/sprite.hpp"

class MirrorFactory : public Factory {
public:
	MirrorFactory(entt::DefaultRegistry& registry);
	virtual ~MirrorFactory();

	std::uint32_t create(float posX, float posY);

private:
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_mirrorSprite;
};
