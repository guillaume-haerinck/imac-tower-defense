#include "attack-system.hpp"

#include <math.h>
#include <spdlog/spdlog.h>

#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/targeting.hpp"
#include "components/look-at.hpp"
#include "components/shoot-at.hpp"
#include "components/shoot-laser.hpp"
#include "components/health.hpp"
#include "components/shake.hpp"
#include "components/velocity.hpp"
#include "events/enemy-damaged.hpp"
#include "events/laser-particle-dead.hpp"
#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"
#include "services/random/i-random.hpp"

AttackSystem::AttackSystem(entt::DefaultRegistry& registry, EventEmitter& emitter) : ISystem(registry, emitter), m_projectileFactory(registry), m_explosionFactory(registry) {
	m_emitter.on<evnt::LaserParticleDead>([this](const evnt::LaserParticleDead & event, EventEmitter & emitter) {
		IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
		if (randomService.random() < 0.1) {
			trySpawnLaserParticle(event.position, 1);
			trySpawnLaserParticle(event.position, 1);
		}
	});
}

void AttackSystem::update(float deltatime) {

	// Look at target
	m_registry.view<cmpt::Targeting, cmpt::Transform , cmpt::LookAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, cmpt::LookAt lookAt) {
		if ( m_registry.valid(targeting.targetId) ) {
			glm::vec2 direction = m_registry.get<cmpt::Transform>(targeting.targetId).position - transform.position;
			transform.rotation = atan2(direction.y, direction.x);
		}
	});

	// Shoot a projectile at target
	m_registry.view<cmpt::Targeting, cmpt::Transform, cmpt::ShootAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, cmpt::ShootAt & shootAt) {
		if (m_registry.valid(targeting.targetId) ) {
			if (shootAt.timer == shootAt.loadingTime) {
				if (m_registry.has<projectileType::Slow>(entity)) {
					m_projectileFactory.createSlow(transform.position, targeting.targetId);
				}
				if (m_registry.has<projectileType::Damage>(entity)) {
					m_projectileFactory.createDamage(transform.position, targeting.targetId);
				}
				shootAt.timer = 0;
			}
			else {
				shootAt.timer++;
			}
		}
	});

	// Pick a new target if current one is out of range or dead
	m_registry.view<cmpt::Transform, cmpt::Targeting, entityTag::Tower>().each([this](auto entity1, cmpt::Transform & transform1, cmpt::Targeting & targeting,auto) {
		//Damage towers
		if (m_registry.has<projectileType::Damage>(entity1)) {
			if (!m_registry.valid(targeting.targetId) || !this->isInRange(transform1, targeting.targetingRange, targeting.targetId)) {
				targeting.targetId = -1;
				m_registry.view<cmpt::Transform, cmpt::Hitbox, entityTag::Enemy>().each([this, entity1, transform1, &targeting](auto entity2, cmpt::Transform & transform2, cmpt::Hitbox & enemyHitbox, auto) {
					if (this->isInRange(transform1, targeting.targetingRange, transform2, enemyHitbox.radius)) {
						targeting.targetId = entity2;
					}
				});
			}
		}
		//Slow towers pick a new target once they have slowed the current one
		else if (m_registry.has<projectileType::Slow>(entity1)) {
			if (!m_registry.valid(targeting.targetId) || !this->isInRange(transform1, targeting.targetingRange, targeting.targetId) || m_registry.get<cmpt::Velocity>(targeting.targetId).velMultiplier < 0.999 ) {
				float maxVelMult = 0;
				targeting.targetId = -1;
				m_registry.view<cmpt::Transform, cmpt::Hitbox, entityTag::Enemy>().each([this, entity1, transform1, &targeting, &maxVelMult](auto entity2, cmpt::Transform & transform2, cmpt::Hitbox & enemyHitbox, auto) {
					if (this->isInRange(transform1, targeting.targetingRange, transform2, enemyHitbox.radius) ) {
						float velMult = m_registry.get<cmpt::Velocity>(entity2).velMultiplier;
						if (velMult > maxVelMult) {
							targeting.targetId = entity2;
							maxVelMult = velMult;
						}
					}
				});
			}
		}
	});

	// Shoot laser
	glLineWidth(LASER_WIDTH);
	m_registry.view<cmpt::ShootLaser, cmpt::Transform>().each([this, deltatime](auto entity, cmpt::ShootLaser & laser, cmpt::Transform & transform) {
		this->shootLaser(transform.position, transform.rotation, 31, entity, deltatime, m_registry.has<stateTag::IsBeingControlled>(entity) || !laser.isActiv);
	});
	glLineWidth(1);
}

/* ---------------------------- PRIVATE METHODS ------------------------------- */

void AttackSystem::shootLaser(glm::vec2 pos, float agl, int nbBounce , unsigned int launcherId, float deltatime, bool isTransparent) {

	glm::vec2 tlCorner = glm::vec2(0, PROJ_HEIGHT);
	glm::vec2 trCorner = glm::vec2(PROJ_WIDTH_RAT, PROJ_HEIGHT);
	glm::vec2 brCorner = glm::vec2(PROJ_WIDTH_RAT, 0);
	glm::vec2 blCorner = glm::vec2(0, 0);

	glm::vec2 unitDirVector = glm::vec2(cos(agl), sin(agl));
	glm::vec2 posPlusUnitVector = pos + unitDirVector;

	glm::vec2 topInter = imaths::segmentsIntersection(pos, posPlusUnitVector, tlCorner, trCorner);
	glm::vec2 rightInter = imaths::segmentsIntersection(pos, posPlusUnitVector, trCorner, brCorner);
	glm::vec2 botInter = imaths::segmentsIntersection(pos, posPlusUnitVector, brCorner, blCorner);
	glm::vec2 leftInter = imaths::segmentsIntersection(pos, posPlusUnitVector, blCorner, tlCorner);

	glm::vec2 laserEnd;
	float surfaceAngle;

	float t = std::numeric_limits<float>::infinity();
	bool mirrorIsBeingConstructed = false;
	//Mirrors
	m_registry.view<cmpt::Transform, cmpt::Hitbox, entityTag::Mirror>().each([this,&laserEnd,&surfaceAngle,&t,pos, unitDirVector, posPlusUnitVector, &mirrorIsBeingConstructed](auto entity, cmpt::Transform & mirrorTransform, cmpt::Hitbox& trigger, auto) {
		glm::vec2 mirrorPos = mirrorTransform.position;
		glm::vec2 mirrorDir = glm::vec2(cos(mirrorTransform.rotation), sin(mirrorTransform.rotation));
		glm::vec2 inter = imaths::segmentsIntersection(pos, posPlusUnitVector, mirrorPos-trigger.radius*mirrorDir, mirrorPos+trigger.radius*mirrorDir);
		if ( 0 <= inter.x && inter.x < t && 0 <= inter.y && inter.y <= 1) {
			t = inter.x;
			laserEnd = pos + t*unitDirVector;
			surfaceAngle = mirrorTransform.rotation;
			mirrorIsBeingConstructed = m_registry.has<stateTag::IsBeingControlled>(entity);
		}
	});

	if (t == std::numeric_limits<float>::infinity()) {
		nbBounce = 0;
		//Walls
		if (0 <= topInter.y && topInter.y <= 1 && topInter.x >= 0) {
			laserEnd = tlCorner + topInter.y * (trCorner - tlCorner);
			surfaceAngle = 0;
		}
		if (0 <= rightInter.y && rightInter.y <= 1 && rightInter.x >= 0) {
			laserEnd = trCorner + rightInter.y * (brCorner - trCorner);
			surfaceAngle = imaths::TAU / 4;
		}
		if (0 <= botInter.y && botInter.y <= 1 && botInter.x >= 0) {
			laserEnd = brCorner + botInter.y * (blCorner - brCorner);
			surfaceAngle = 0;
		}
		if (0 <= leftInter.y && leftInter.y <= 1 && leftInter.x >= 0) {
			laserEnd = blCorner + leftInter.y * (tlCorner - blCorner);
			surfaceAngle = imaths::TAU / 4;
		}
	}
	else {
		trySpawnLaserParticle(laserEnd, deltatime);
	}

	//Damage entities
	if (!isTransparent) {
		float laserLength = sqrt((pos.x - laserEnd.x)*(pos.x - laserEnd.x) + (pos.y - laserEnd.y)*(pos.y - laserEnd.y));
		glm::vec2 normal = glm::vec2(laserEnd.y - pos.y, pos.x - laserEnd.x);
		normal /= glm::length(normal);
		m_registry.view<cmpt::Transform, cmpt::Hitbox, cmpt::Health>().each([this, normal, launcherId, pos, laserEnd, laserLength, deltatime](auto entity, cmpt::Transform & targetTransform, cmpt::Hitbox& targetTrigger, cmpt::Health& targetHealth) {
			if (!m_registry.has<stateTag::IsBeingControlled>(entity)) {
				float orthoComp = abs(normal.x*(targetTransform.position.x - pos.x) + normal.y*(targetTransform.position.y - pos.y));
				float colinComp = ((laserEnd.x - pos.x)*(targetTransform.position.x - pos.x) + (laserEnd.y - pos.y)*(targetTransform.position.y - pos.y)) / laserLength;
				if (0 <= colinComp && colinComp <= laserLength && orthoComp < targetTrigger.radius && launcherId != entity) {
					m_emitter.publish<evnt::EnemyDamaged>(entity, targetTransform.position, LASER_DAMAGE_PER_SECOND*deltatime);
					trySpawnLaserParticle(targetTransform.position, deltatime);
				}
			}
		});
	}

	IDebugDraw & debugDraw = locator::debugDraw::ref();
	float alpha = isTransparent ? 0.25 : 1;
	debugDraw.setColor(122, 249, 237, alpha);
	debugDraw.line(pos.x, pos.y, laserEnd.x, laserEnd.y,LASER);
	if (nbBounce > 0) {
		shootLaser(laserEnd - unitDirVector * 0.001f, 2 * surfaceAngle - agl, nbBounce - 1 , -1, deltatime, isTransparent || mirrorIsBeingConstructed);
	}
}

void AttackSystem::trySpawnLaserParticle(glm::vec2 pos, float deltatime) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	if (randomService.random() < 12 * deltatime) {
		m_explosionFactory.createLaserParticle(pos, randomService.random(imaths::TAU));
	}
}

bool AttackSystem::isInRange(cmpt::Transform transform1, float radius1, cmpt::Transform transform2, float radius2) {
	const glm::vec2 deltaPos = transform2.position - transform1.position;
	const float distanceSq = deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y;
	const float radii = radius1 + radius2;
	return distanceSq <= radii * radii;
}

bool AttackSystem::isInRange(cmpt::Transform transform1, float radius1, unsigned int targetId) {
	cmpt::Transform transform2 = m_registry.get<cmpt::Transform>(targetId);
	float radius2 = m_registry.get<cmpt::Hitbox>(targetId).radius;
	return isInRange(transform1, radius1, transform2, radius2);
}