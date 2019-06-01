#pragma once

#include "i-system.hpp"
#include "entity-factories/projectile-factory.hpp"

#include "components/hitbox.hpp"
#include "components/transform.hpp"

#include "events/handlers/event-emitter.hpp"
#include "events/inputs/mouse-move.hpp"

#include "entity-factories/vfx-factory.hpp"

class AttackSystem : public ISystem {
public:
	AttackSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(float deltatime) override;

private:
	ProjectileFactory m_projectileFactory;

private:
	void shootLaser(glm::vec2 pos, float agl, int nbBounce, unsigned int launcherId, float deltatime, bool isTransparent, glm::vec3 col, float launcherAlpha);
	void trySpawnLaserParticle(glm::vec2 pos, float deltatime);
	void glowOnMirror(glm::vec2 pos);
	bool isInRange(cmpt::Transform transform1, float radius1, cmpt::Transform transform2, float radius2);
	bool isInRange(cmpt::Transform transform1, float radius1, unsigned int targetId);
	VFXFactory m_vfxFactory;
};
