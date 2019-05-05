#include "laser-system.hpp"

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "components/transform.hpp"
#include "components/shoot-laser.hpp"

#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"

#include "core/maths.hpp"

#include <spdlog/spdlog.h>

LaserSystem::LaserSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
	: System(registry), m_emitter(emitter)
{
}


void LaserSystem::update(double deltatime) {
    m_registry.view<cmpt::ShootLaser, cmpt::Transform>().each([this,deltatime](auto entity, cmpt::ShootLaser& laser, cmpt::Transform& transform) {
		shootLaser(transform.position, transform.rotation,4);
    });
}

void LaserSystem::shootLaser(glm::vec2 pos, float agl, int nbBounce) {
	glm::vec2 tlCorner = glm::vec2(0, PROJ_HEIGHT);
	glm::vec2 trCorner = glm::vec2(PROJ_WIDTH_RAT, PROJ_HEIGHT);
	glm::vec2 brCorner = glm::vec2(PROJ_WIDTH_RAT, 0);
	glm::vec2 blCorner = glm::vec2(0, 0);

	glm::vec2 unitDirVector = glm::vec2(cos(agl), sin(agl));
	glm::vec2 posPlusUnitVector = pos + unitDirVector ;

	glm::vec2 topInter = imac::segmentsIntersection(pos, posPlusUnitVector, tlCorner, trCorner);
	glm::vec2 rightInter = imac::segmentsIntersection(pos, posPlusUnitVector, trCorner, brCorner);
	glm::vec2 botInter = imac::segmentsIntersection(pos, posPlusUnitVector, brCorner, blCorner);
	glm::vec2 leftInter = imac::segmentsIntersection(pos, posPlusUnitVector, blCorner, tlCorner);

	glm::vec2 laserEnd;
	float surfaceAngle;

	if (0 <= topInter.y && topInter.y <= 1 && topInter.x >=0 ) {
		laserEnd = tlCorner + topInter.y * (trCorner - tlCorner);
		surfaceAngle = 0;
	}
	if (0 <= rightInter.y && rightInter.y <= 1 && rightInter.x >= 0 ) {
		laserEnd = trCorner + rightInter.y * (brCorner - trCorner);
		surfaceAngle = imac::TAU/4;
	}
	if (0 <= botInter.y && botInter.y <= 1 && botInter.x >= 0) {
		laserEnd = brCorner + botInter.y * (blCorner - brCorner);
		surfaceAngle = 0;
	}
	if (0 <= leftInter.y && leftInter.y <= 1 && leftInter.x >= 0) {
		laserEnd = blCorner + leftInter.y * (tlCorner - blCorner);
		surfaceAngle = imac::TAU / 4;
	}

	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setColor(255, 0, 100, 1);
	debugDraw.line(pos.x,pos.y, laserEnd.x, laserEnd.y);
	if (nbBounce > 0) {
		shootLaser(laserEnd - unitDirVector*0.001f, 2*surfaceAngle -agl , nbBounce - 1);
	}
}