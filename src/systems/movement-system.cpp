#include "movement-system.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

#include <spdlog/spdlog.h>
#include <math.h> 

#include "core/maths.hpp"
#include "core/constants.hpp"
#include "core/tags.hpp"
#include "core/level/graph.hpp"
#include "events/enemy-damaged.hpp"
#include "events/interactions/select-rotation.hpp"
#include "components/transform.hpp"
#include "components/rigid-body.hpp"
#include "components/look-at.hpp"
#include "components/look-at-mouse.hpp"
#include "components/rotated-by-mouse.hpp"
#include "components/constrained-rotation.hpp"
#include "components/trajectory.hpp"
#include "components/follow.hpp"
#include "components/pathfinding.hpp"
#include "components/targeting.hpp"
#include "components/move-towards-mouse.hpp"
#include "components/attached-to.hpp"
#include "components/velocity.hpp"
#include "components/direction.hpp"
#include "components/hitbox.hpp"

MovementSystem::MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: ISystem(registry, emitter)
{
	// TODO cleanup this code, some of this logic can be calculated when publishing the event
	m_emitter.on<evnt::SelectRotation>([this](const evnt::SelectRotation & event, EventEmitter & emitter) {
		m_registry.view<cmpt::Transform, cmpt::RotatedByMouse>().each([this, event](auto entity, cmpt::Transform & transform, cmpt::RotatedByMouse & rotatedByMouse) {
			glm::vec2 toPrevMouse = m_prevMousePos * glm::vec2(WIN_RATIO, 1.0f) - transform.position;
			glm::vec2 toMouse = event.mousePos * glm::vec2(WIN_RATIO, 1.0f) - transform.position;
			float deltaAgl = atan2(toMouse.y, toMouse.x) - atan2(toPrevMouse.y, toPrevMouse.x);
			if (m_registry.has<cmpt::ConstrainedRotation>(entity)) {
				cmpt::ConstrainedRotation& rot = m_registry.get<cmpt::ConstrainedRotation>(entity);
				rot.accumulatedAngle += deltaAgl;
				while (rot.accumulatedAngle > rot.angleStep) {
					rot.accumulatedAngle -= rot.angleStep;
					transform.rotation += rot.angleStep;
				}
				while (rot.accumulatedAngle < -rot.angleStep) {
					rot.accumulatedAngle += rot.angleStep;
					transform.rotation -= rot.angleStep;
				}
			}
			else {
				transform.rotation += deltaAgl;
			}
		});
		//Look at mouse
		m_registry.view<cmpt::Transform, cmpt::LookAtMouse>().each([this, event](auto entity, cmpt::Transform & transform, cmpt::LookAtMouse & lookAtMouse) {
			float agl = atan2(event.mousePos.y - transform.position.y, event.mousePos.x * WIN_RATIO - transform.position.x);
			if (m_registry.has<cmpt::ConstrainedRotation>(entity)) {
				cmpt::ConstrainedRotation& rot = m_registry.get<cmpt::ConstrainedRotation>(entity);
				transform.rotation = round(agl / rot.angleStep) * rot.angleStep + lookAtMouse.angleOffset;
			}
			else {
				transform.rotation = agl + lookAtMouse.angleOffset;
			}
		});
		//Update previous mouse position
		m_prevMousePos = event.mousePos;
	});
}

void MovementSystem::update(float deltatime) {
	//Update velocity multiplier
	m_registry.view<cmpt::Velocity>().each([this, deltatime](auto entity, cmpt::Velocity& velocity) {
		velocity.multiplierAge += deltatime;
		if( velocity.multiplierAge > velocity.multiplierLifespan){
			velocity.multiplierLifespan = 1;
			velocity.multiplierAge = 0;
			velocity.velMultiplier = 1;
		}
	});

	m_registry.view<cmpt::RigidBody, cmpt::Transform>().each([](auto entity, cmpt::RigidBody & rigidbody, cmpt::Transform & transform) {
		transform.position.x = rigidbody.body->GetPosition().x;
		transform.position.y = rigidbody.body->GetPosition().y;
		transform.rotation = rigidbody.body->GetAngle();
	});

	m_registry.view<cmpt::Transform, cmpt::Trajectory>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Trajectory& traj) {
		glm::vec2 direction = traj.traj.at(traj.currentTarget) - transform.position;
		float norm = glm::length(direction);
		if (norm > 1) {
			direction *= 0.5 / glm::length(direction);
			transform.position += direction;
		}
		else if (traj.currentTarget < traj.traj.size() - 1) {
			traj.currentTarget++;
		}
	});

	m_registry.view<cmpt::Transform, cmpt::Direction, cmpt::Velocity>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Direction& direction, cmpt::Velocity& velocity) {
		transform.position += direction.dir*velocity.velocity*velocity.velMultiplier*deltatime;
	});


	m_registry.view<cmpt::Transform, cmpt::Velocity, cmpt::Pathfinding>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Velocity& velocity, cmpt::Pathfinding& pathfinding) {
		Level* level = pathfinding.level;
		glm::vec2 direction = level->getNodePosition(pathfinding.currentTarget) - transform.position;
		float norm = glm::length(direction);
		if (norm > 1) {
			direction /= norm;
			float vel = deltatime*velocity.velocity*imaths::rangeMapping(velocity.multiplierAge,0,velocity.multiplierLifespan,velocity.velMultiplier,1);
			transform.position += vel*direction;
		}
		else if (pathfinding.currentTarget != level->getGraph()->getEndNode()) {
			int tmp = pathfinding.currentTarget;
			pathfinding.currentTarget = level->getPathfindingGraph()->pickNextNode(pathfinding.currentTarget,pathfinding.previousNode);
			pathfinding.previousNode = tmp;
		}
		else {
			m_registry.destroy(entity);
		}
	});

	m_registry.view<cmpt::Transform, cmpt::Follow, cmpt::Targeting>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Follow& follow, cmpt::Targeting& targeting) {
		if (m_registry.valid(targeting.targetId)) {
			glm::vec2 targetPosition = m_registry.get<cmpt::Transform>(targeting.targetId).position;
			glm::vec2 direction = targetPosition - transform.position;
			float norm = glm::length(direction);
			if (norm > m_registry.get<cmpt::Hitbox>(targeting.targetId).radius) {
				direction *= follow.velocity / glm::length(direction);
				transform.position += direction;
			}
			else {
				if (m_registry.has<projectileType::Damage>(entity)) {
					m_emitter.publish<evnt::EnemyDamaged>(targeting.targetId, targetPosition, 1.0f);
					m_registry.destroy(entity);
				}
				if (m_registry.has<projectileType::Slow>(entity)) {
					cmpt::Velocity& vel = m_registry.get<cmpt::Velocity>(targeting.targetId);
					vel.multiplierAge = 0;
					vel.velMultiplier = 0.3;
					vel.multiplierLifespan = 4;
					m_registry.destroy(entity);
				}
			}
		}
		else {
			m_registry.destroy(entity);
		}
	});
}
