#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "component-factories/rigid-body-factory.hpp"

class TowerFactory : public Factory {
public:
	TowerFactory(entt::DefaultRegistry& registry, b2World& physicWorld);

	void create(float posX, float posY);

private:
	SpriteFactory m_spriteFactory;
	RigidBodyFactory m_rigidBodyFactory;
};
