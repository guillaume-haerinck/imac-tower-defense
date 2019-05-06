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
#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"

AttackSystem::AttackSystem(entt::DefaultRegistry& registry, EventEmitter& emitter) : ISystem(registry), m_projectileFactory(registry), m_emitter(emitter) {}

void AttackSystem::connectEvents() {
	if (m_bConnected == false) {
		auto connection = m_emitter.on<evnt::MouseMove>([this](const evnt::MouseMove & event, EventEmitter & emitter) {
			m_registry.view<entityTag::Tower, cmpt::Transform>().each([this,event](auto entity, auto, cmpt::Transform& transform) {
				float agl = atan2(event.mousePos.y-transform.position.y, event.mousePos.x*WIN_RATIO- transform.position.x);
				spdlog::info(agl);
				transform.rotation = agl;
			});
		});
		m_mouseMoveCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::MouseMove>>(connection);

		m_bConnected = true;
	}
}

void AttackSystem::disconnectEvents() {
	if (m_bConnected == true) {
		m_emitter.erase(*m_mouseMoveCon);
		m_mouseMoveCon.reset();

		m_bConnected = false;
	}
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
				m_projectileFactory.create(transform.position, targeting.targetId);
				shootAt.timer = 0;
			}
			else {
				shootAt.timer++;
			}
		}
	});

	// Pick a new target if current one is out of range or dead
	m_registry.view<cmpt::Transform, cmpt::Trigger, cmpt::Targeting>().each([this](auto entity1, cmpt::Transform & transform1, cmpt::Trigger & trigger1, cmpt::Targeting & targeting) {
		IDebugDraw& dd = entt::ServiceLocator<IDebugDraw>::ref();
		dd.DrawCircle(b2Vec2(transform1.position.x, transform1.position.y), trigger1.radius, b2Color(1, 0, 0, 0.5f));

		if ( !m_registry.valid(targeting.targetId) || !isInRange(transform1, trigger1, targeting.targetId)) {
			targeting.targetId = -1;
			m_registry.view<cmpt::Transform, cmpt::Trigger, entityTag::Enemy>().each([this, entity1, transform1, trigger1, &targeting](auto entity2, cmpt::Transform & transform2, cmpt::Trigger & trigger2, auto) {
				if ( isInRange(transform1,trigger1,transform2,trigger2) ) {
					targeting.targetId = entity2;
				}
			});
		}
	});

	// Shoot laser
	m_registry.view<cmpt::ShootLaser, cmpt::Transform>().each([this, deltatime](auto entity, cmpt::ShootLaser & laser, cmpt::Transform & transform) {
		shootLaser(transform.position, transform.rotation, 4);
	});
}

/* ---------------------------- PRIVATE METHODS ------------------------------- */

void AttackSystem::shootLaser(glm::vec2 pos, float agl, int nbBounce) {
	glm::vec2 tlCorner = glm::vec2(0, PROJ_HEIGHT);
	glm::vec2 trCorner = glm::vec2(PROJ_WIDTH_RAT, PROJ_HEIGHT);
	glm::vec2 brCorner = glm::vec2(PROJ_WIDTH_RAT, 0);
	glm::vec2 blCorner = glm::vec2(0, 0);

	glm::vec2 unitDirVector = glm::vec2(cos(agl), sin(agl));
	glm::vec2 posPlusUnitVector = pos + unitDirVector;

	glm::vec2 topInter = imac::segmentsIntersection(pos, posPlusUnitVector, tlCorner, trCorner);
	glm::vec2 rightInter = imac::segmentsIntersection(pos, posPlusUnitVector, trCorner, brCorner);
	glm::vec2 botInter = imac::segmentsIntersection(pos, posPlusUnitVector, brCorner, blCorner);
	glm::vec2 leftInter = imac::segmentsIntersection(pos, posPlusUnitVector, blCorner, tlCorner);

	glm::vec2 laserEnd;
	float surfaceAngle;

	//Walls

	if (0 <= topInter.y && topInter.y <= 1 && topInter.x >= 0) {
		laserEnd = tlCorner + topInter.y * (trCorner - tlCorner);
		surfaceAngle = 0;
	}
	if (0 <= rightInter.y && rightInter.y <= 1 && rightInter.x >= 0) {
		laserEnd = trCorner + rightInter.y * (brCorner - trCorner);
		surfaceAngle = imac::TAU / 4;
	}
	if (0 <= botInter.y && botInter.y <= 1 && botInter.x >= 0) {
		laserEnd = brCorner + botInter.y * (blCorner - brCorner);
		surfaceAngle = 0;
	}
	if (0 <= leftInter.y && leftInter.y <= 1 && leftInter.x >= 0) {
		laserEnd = blCorner + leftInter.y * (tlCorner - blCorner);
		surfaceAngle = imac::TAU / 4;
	}

	float t = std::numeric_limits<float>::infinity();
	//Mirrors
	m_registry.view<cmpt::Transform, cmpt::Trigger, entityTag::Mirror>().each([this,&laserEnd,&surfaceAngle,&t,pos, unitDirVector, posPlusUnitVector](auto entity, cmpt::Transform & mirrorTransform, cmpt::Trigger& trigger, auto) {
		glm::vec2 mirrorPos = mirrorTransform.position;
		glm::vec2 mirrorDir = glm::vec2(cos(mirrorTransform.rotation), sin(mirrorTransform.rotation));
		glm::vec2 inter = imac::segmentsIntersection(pos, posPlusUnitVector, mirrorPos-trigger.radius*mirrorDir, mirrorPos+trigger.radius*mirrorDir);
		if ( 0 <= inter.x && inter.x < t && 0 <= inter.y && inter.y <= 1) {
			t = inter.x;
			laserEnd = pos + t*unitDirVector;
			surfaceAngle = mirrorTransform.rotation;
		}
	});


	IDebugDraw & debugDraw = locator::debugDraw::ref();
	debugDraw.setColor(255, 0, 100, 1);
	debugDraw.line(pos.x, pos.y, laserEnd.x, laserEnd.y);
	if (nbBounce > 0) {
		shootLaser(laserEnd - unitDirVector * 0.001f, 2 * surfaceAngle - agl, nbBounce - 1);
	}
}

bool AttackSystem::isInRange(cmpt::Transform transform1, cmpt::Trigger trigger1, cmpt::Transform transform2, cmpt::Trigger trigger2) {
	const glm::vec2 deltaPos = transform2.position - transform1.position;
	const float distanceSq = deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y;
	const float radii = trigger1.radius + trigger2.radius;
	return distanceSq <= radii * radii;
}

bool AttackSystem::isInRange(cmpt::Transform transform1, cmpt::Trigger trigger1, unsigned int targetId) {
	cmpt::Transform transform2 = m_registry.get<cmpt::Transform>(targetId);
	cmpt::Trigger trigger2 = m_registry.get<cmpt::Trigger>(targetId);
	return isInRange(transform1, trigger1, transform2, trigger2);
}