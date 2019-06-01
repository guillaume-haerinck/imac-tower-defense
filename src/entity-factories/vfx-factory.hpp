#pragma once

#include <entt/entt.hpp>

#include "factory.hpp"
#include "component-factories/sprite-factory.hpp"
#include "components/sprite.hpp"

class VFXFactory : public Factory {
public:
	VFXFactory(entt::DefaultRegistry& registry);
	virtual ~VFXFactory();

	void createExplosion(glm::vec2 pos, ShaderType type);
	void createLaserParticle(glm::vec2 pos, float dirAgl);

private:
	SpriteFactory m_spriteFactory;
	cmpt::Sprite m_towerExplosionSprite;
	cmpt::Sprite m_enemyExplosionSprite;
	cmpt::Sprite m_laserParticleSprite;
};

