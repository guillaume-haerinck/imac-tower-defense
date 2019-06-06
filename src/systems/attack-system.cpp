#include "attack-system.hpp"

#include <math.h>
#include <spdlog/spdlog.h>

#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/targeting.hpp"
#include "components/shoot-at.hpp"
#include "components/shoot-laser.hpp"
#include "components/health.hpp"
#include "components/shake.hpp"
#include "components/velocity.hpp"
#include "components/tint-colour.hpp"
#include "events/entity-damaged.hpp"
#include "events/laser-particle-dead.hpp"
#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"
#include "services/random/i-random.hpp"
#include "services/helper/i-helper.hpp"
#include "components/animated.hpp"
#include <SDL2/SDL.h>

AttackSystem::AttackSystem(entt::DefaultRegistry& registry, EventEmitter& emitter) : ISystem(registry, emitter), m_projectileFactory(registry), m_vfxFactory(registry) {
	m_emitter.on<evnt::LaserParticleDead>([this](const evnt::LaserParticleDead & event, EventEmitter & emitter) {
		IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
		if (randomService.random() < 0.1) {
			this->trySpawnLaserParticle(event.position, 1);
			this->trySpawnLaserParticle(event.position, 1);
		}
	});
}

void AttackSystem::update(float deltatime) {

	// Look at target
	m_registry.view<cmpt::Targeting, cmpt::Transform , targetingTag::LookAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, auto) {
		if ( m_registry.valid(targeting.targetId) ) {
			glm::vec2 direction = m_registry.get<cmpt::Transform>(targeting.targetId).position - transform.position;
			transform.rotation = atan2(direction.y, direction.x);
		}
	});

	// Shoot a projectile at target
	m_registry.view<cmpt::Targeting, cmpt::ShootAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::ShootAt & shootAt) {
		if (m_registry.valid(targeting.targetId) ) {
			if (shootAt.timer == shootAt.loadingTime) {
				IHelper& helper = entt::ServiceLocator<IHelper>::ref();
				if (m_registry.has<projectileType::Slow>(entity)) {
					m_projectileFactory.createSlow(helper.getPosition(entity), targeting.targetId);
				}
				if (m_registry.has<projectileType::Damage>(entity)) {
					m_projectileFactory.createDamage(helper.getPosition(entity), targeting.targetId);
				}
				shootAt.timer = 0;
			}
			else {
				shootAt.timer++;
			}
		}
	});

	// Pick a new target if current one is out of range or dead
	m_registry.view<cmpt::Transform, cmpt::Targeting, entityTag::Tower>().each([this,deltatime](auto entity1, cmpt::Transform & transform1, cmpt::Targeting & targeting,auto) {
		if (!m_registry.has<stateTag::IsBeingControlled>(entity1)) {
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
				if (!m_registry.valid(targeting.targetId) || !this->isInRange(transform1, targeting.targetingRange, targeting.targetId) || m_registry.get<cmpt::Velocity>(targeting.targetId).velMultiplier < 0.999) {
					float maxVelMult = 0;
					targeting.targetId = -1;
					m_registry.view<cmpt::Transform, cmpt::Hitbox, entityTag::Enemy>().each([this, deltatime, entity1, transform1, &targeting, &maxVelMult](auto entity2, cmpt::Transform & transform2, cmpt::Hitbox & enemyHitbox, auto) {
						if (this->isInRange(transform1, targeting.targetingRange, transform2, enemyHitbox.radius)) {
							cmpt::Velocity& velocity = m_registry.get<cmpt::Velocity>(entity2);
							float velMult = deltatime * velocity.velocity*imaths::rangeMapping(velocity.multiplierAge, 0, velocity.multiplierLifespan, velocity.velMultiplier, 1);;
							if (velMult > maxVelMult) {
								targeting.targetId = entity2;
								maxVelMult = velMult;
							}
						}
					});
				}
			}
		}
	});

	// Shoot laser
	glLineWidth(LASER_WIDTH);
	m_registry.view<cmpt::ShootLaser, cmpt::Transform>().each([this, deltatime](auto entity, cmpt::ShootLaser & laser, cmpt::Transform & transform) {
		IHelper& helper = entt::ServiceLocator<IHelper>::ref();
		glm::vec3 col = laser.col;
		if (helper.mouseIsOn(entity)) {
			col *= 0.5;
		}
		this->shootLaser(helper.getPositionTowerTip(entity), transform.rotation, 31, entity, deltatime, !laser.isActiv,col,helper.getAlpha(entity));
	});
	glLineWidth(1);
}

/* ---------------------------- PRIVATE METHODS ------------------------------- */

void AttackSystem::shootLaser(glm::vec2 pos, float agl, int nbBounce , unsigned int launcherId, float deltatime, bool isTransparent, glm::vec3 col, float launcherAlpha) {
	IHelper& helper = entt::ServiceLocator<IHelper>::ref();

	glm::vec2 unitDirVector = glm::vec2(cos(agl), sin(agl));
	glm::vec2 posPlusUnitVector = pos + unitDirVector;

	glm::vec2 laserEnd;
	float surfaceAngle;

	std::uint32_t nextLauncherId = entt::null;

	float t = std::numeric_limits<float>::infinity();
	bool mirrorIsBeingControlled = false;
	bool arrivedOnMirrorEdge = false;
	//Mirrors
	m_registry.view<cmpt::Transform, cmpt::Hitbox, entityTag::Mirror>().each([this,&nextLauncherId,&laserEnd,&surfaceAngle,&t,pos, unitDirVector, posPlusUnitVector, &mirrorIsBeingControlled, &helper, &arrivedOnMirrorEdge](auto mirror, cmpt::Transform & mirrorTransform, cmpt::Hitbox& trigger, auto) {
		glm::vec2 mirrorPos = helper.getPosition(mirror);
		glm::vec2 mirrorDir = glm::vec2(cos(mirrorTransform.rotation), sin(mirrorTransform.rotation));
		glm::vec2 inter = imaths::segmentsIntersection(pos, posPlusUnitVector, mirrorPos-trigger.radius*mirrorDir, mirrorPos+trigger.radius*mirrorDir);
		if ( 0 <= inter.x && inter.x < t && 0 <= inter.y && inter.y <= 1) {
			t = inter.x;
			laserEnd = pos + t*unitDirVector;
			surfaceAngle = mirrorTransform.rotation;
			mirrorIsBeingControlled = m_registry.has<stateTag::IsBeingControlled>(mirror) || m_registry.has<positionTag::IsOnHoveredTile>(mirror);
			nextLauncherId = mirror;
			arrivedOnMirrorEdge = false;
		}
		else { //Check if they were actually aligned
			if (inter.x == std::numeric_limits<float>::infinity()) { //The were parallel.
				glm::vec2 mirrorPt1 = mirrorPos + MIRROR_RADIUS * mirrorDir;
				glm::vec2 mirrorPt2 = mirrorPos - MIRROR_RADIUS * mirrorDir;
				glm::vec2 dir1 = mirrorPt1 - pos;
				glm::vec2 dir2 = mirrorPt2 - pos;
				if (dir1.x*unitDirVector.x + dir2.y*unitDirVector.y > 0) { //The mirror is in the right direction
					if (abs(dir1.x*dir2.y - dir2.x*dir1.y) < 20) { //The were aligned 
						float dSq1 = dir1.x*dir1.x + dir1.y*dir1.y;
						float dSq2 = dir2.x*dir2.x + dir2.y*dir2.y;
						if (dSq1 < dSq2) {
							t = sqrt(dSq1);
							laserEnd = mirrorPt1;
						}
						else {
							t = sqrt(dSq2);
							laserEnd = mirrorPt2;
						}
						/*glm::vec2 dir = mirrorPos - pos;
						float distToMirrorCenter = sqrt(dir.x*dir.x + dir.y*dir.y);
						t = distToMirrorCenter - MIRROR_RADIUS;
						laserEnd = pos + (1- MIRROR_RADIUS/ distToMirrorCenter)*dir;*/
						mirrorIsBeingControlled = m_registry.has<stateTag::IsBeingControlled>(mirror) || m_registry.has<positionTag::IsOnHoveredTile>(mirror);
						nextLauncherId = mirror;
						arrivedOnMirrorEdge = true;
					}
				}
			}
		}
	});

	if (t == std::numeric_limits<float>::infinity()) {
		nbBounce = 0;
		//Walls

		glm::vec2 tlCorner = glm::vec2(0, PROJ_HEIGHT);
		glm::vec2 trCorner = glm::vec2(PROJ_WIDTH_RAT, PROJ_HEIGHT);
		glm::vec2 brCorner = glm::vec2(PROJ_WIDTH_RAT, 0);
		glm::vec2 blCorner = glm::vec2(0, 0);

		glm::vec2 topInter = imaths::segmentsIntersection(pos, posPlusUnitVector, tlCorner, trCorner);
		glm::vec2 rightInter = imaths::segmentsIntersection(pos, posPlusUnitVector, trCorner, brCorner);
		glm::vec2 botInter = imaths::segmentsIntersection(pos, posPlusUnitVector, brCorner, blCorner);
		glm::vec2 leftInter = imaths::segmentsIntersection(pos, posPlusUnitVector, blCorner, tlCorner);

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
	else { //Effect on mirror bounce
		if (!mirrorIsBeingControlled && !isTransparent) {
			//this->trySpawnLaserParticle(laserEnd, deltatime);
			glowOnMirror(laserEnd);
		}
	}

	//Damage entities
	float laserLength = sqrt((pos.x - laserEnd.x)*(pos.x - laserEnd.x) + (pos.y - laserEnd.y)*(pos.y - laserEnd.y));
	glm::vec2 normal = glm::vec2(laserEnd.y - pos.y, pos.x - laserEnd.x);
	normal /= glm::length(normal);
	m_registry.view<cmpt::Transform, cmpt::Hitbox, cmpt::Health>().each([this, normal, launcherId, pos, laserEnd, laserLength, deltatime, isTransparent, &helper](auto entity, cmpt::Transform & targetTransform, cmpt::Hitbox& targetTrigger, cmpt::Health& targetHealth) {
		glm::vec2 entityPos = helper.getPosition(entity);
		float orthoComp = abs(normal.x*(entityPos.x - pos.x) + normal.y*(entityPos.y - pos.y));
		float colinComp = ((laserEnd.x - pos.x)*(entityPos.x - pos.x) + (laserEnd.y - pos.y)*(entityPos.y - pos.y)) / laserLength;
		if (0 <= colinComp && colinComp <= laserLength && orthoComp < targetTrigger.radius && launcherId != entity) {
			if (m_registry.has<entityTag::Tower>(entity)) {
				m_registry.accommodate<cmpt::TintColour>(entity, glm::vec4(1, 0.4, 0.047, 0.55*(sin(SDL_GetTicks()*0.006) + 1)), true);
			}
			if (!isTransparent && !m_registry.has<stateTag::IsBeingControlled>(entity) && !m_registry.has<cmpt::Animated>(entity)) {
				m_emitter.publish<evnt::EntityDamaged>(entity, LASER_DAMAGE_PER_SECOND*deltatime);
				this->trySpawnLaserParticle(targetTransform.position, deltatime);
			}
		}
	});

	IDebugDraw & debugDraw = locator::debugDraw::ref();
	float alpha = isTransparent ? 0.25 : 1;
	launcherAlpha *= helper.getAlpha(launcherId);
	alpha *= launcherAlpha;
	debugDraw.setColor(col.r,col.g,col.b, alpha);
	debugDraw.line(pos.x, pos.y, laserEnd.x, laserEnd.y, BasicShaderType::LASER);
	if (nbBounce > 0) {
		if (!arrivedOnMirrorEdge) {
			shootLaser(laserEnd - unitDirVector * 0.001f, 2 * surfaceAngle - agl, nbBounce - 1, nextLauncherId, deltatime, isTransparent || mirrorIsBeingControlled, col, launcherAlpha);
		}
		else {
			shootLaser(laserEnd - unitDirVector * 0.001f, agl + imaths::TAU/8, nbBounce - 1, nextLauncherId, deltatime, isTransparent || mirrorIsBeingControlled, col, launcherAlpha);
			shootLaser(laserEnd - unitDirVector * 0.001f, agl - imaths::TAU/8, nbBounce - 1, nextLauncherId, deltatime, isTransparent || mirrorIsBeingControlled, col, launcherAlpha);
		}
	}
}

void AttackSystem::trySpawnLaserParticle(glm::vec2 pos, float deltatime) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	if (randomService.random() < 12 * deltatime) {
		m_vfxFactory.createLaserParticle(pos, randomService.random(imaths::TAU));
	}
}

void AttackSystem::glowOnMirror(glm::vec2 pos) {
	IDebugDraw & debugDraw = locator::debugDraw::ref();
	IRandom & random = locator::random::ref();
	debugDraw.setColor(122, 249, 237,1.0);
	debugDraw.circleWithGlow(pos.x, pos.y, 4+2*0.5*random.noise(SDL_GetTicks()*0.0005));
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