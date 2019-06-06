#include "movement-system.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

#include <spdlog/spdlog.h>
#include <math.h> 

#include "core/maths.hpp"
#include "core/constants.hpp"
#include "core/tags.hpp"
#include "core/level/graph.hpp"
#include "events/entity-damaged.hpp"
#include "events/enemy-reached-end.hpp"
#include "events/interactions/select-rotation.hpp"
#include "components/transform.hpp"
#include "components/rigid-body.hpp"
#include "components/look-at-mouse.hpp"
#include "components/constrained-rotation.hpp"
#include "components/trajectory.hpp"
#include "components/pathfinding.hpp"
#include "components/targeting.hpp"
#include "components/move-towards-mouse.hpp"
#include "components/attached-to.hpp"
#include "components/velocity.hpp"
#include "components/direction.hpp"
#include "components/hitbox.hpp"
#include "components/animated.hpp"
#include "components/sprite-animation.hpp"
#include "components/animation-pixels-vanish.hpp"

MovementSystem::MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: ISystem(registry, emitter)
{
	m_emitter.on<evnt::SelectRotation>([this](const evnt::SelectRotation & event, EventEmitter & emitter) {
		//Look at mouse
		m_registry.view<cmpt::Transform, cmpt::LookAtMouse>().each([this, event](auto entity, cmpt::Transform & transform, cmpt::LookAtMouse & lookAtMouse) {
			float agl = atan2(event.mousePos.y - transform.position.y, event.mousePos.x * WIN_RATIO - transform.position.x);
			if (m_registry.has<cmpt::ConstrainedRotation>(entity)) {
				cmpt::ConstrainedRotation& rot = m_registry.get<cmpt::ConstrainedRotation>(entity);
				rot.angleIndex = ((int)round(agl / rot.angleStep) + rot.nbAngles) % rot.nbAngles;
				// Rotate
				transform.rotation = rot.angleIndex * rot.angleStep + lookAtMouse.angleOffset;
				// Update sprite
				if (m_registry.has<cmpt::SpriteAnimation>(entity)) {
					cmpt::SpriteAnimation& spriteAnim = m_registry.get<cmpt::SpriteAnimation>(entity);
					spriteAnim.activeTile = rot.angleIndex;
					spriteAnim.startTile = rot.angleIndex;
					spriteAnim.endTile = rot.angleIndex;
				}
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
		if (!m_registry.has<cmpt::Animated>(entity)) {
			Level* level = pathfinding.level;
			glm::vec2 direction = level->getNodePosition(pathfinding.currentTarget) - transform.position;
			float norm = glm::length(direction);
			if (norm > 1) {
				direction /= norm;
				float vel = deltatime * velocity.velocity*imaths::rangeMapping(velocity.multiplierAge, 0, velocity.multiplierLifespan, velocity.velMultiplier, 1);
				transform.position += vel * direction;
			}
			else if (pathfinding.currentTarget != level->getGraph()->getEndNode()) {
				int tmp = pathfinding.currentTarget;
				pathfinding.currentTarget = level->getPathfindingGraph()->pickNextNode(pathfinding.currentTarget, pathfinding.previousNode);
				pathfinding.previousNode = tmp;
			}
			else {
				if (m_registry.has<entityTag::Enemy>(entity)) {
					m_registry.assign<cmpt::Animated>(entity, 2, true);
					m_registry.assign<cmpt::AnimationPixelsVanish>(entity, false);
					m_emitter.publish<evnt::EnemyReachedEnd>();
				}
			}
		}
	});

	m_registry.view<cmpt::Transform, cmpt::Velocity, targetingTag::Follow, cmpt::Targeting>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Velocity& vel, auto, cmpt::Targeting& targeting) {
		if (m_registry.valid(targeting.targetId)) {
			glm::vec2 targetPosition = m_registry.get<cmpt::Transform>(targeting.targetId).position;
			glm::vec2 direction = targetPosition - transform.position;
			float norm = glm::length(direction);
			if (norm > m_registry.get<cmpt::Hitbox>(targeting.targetId).radius) {
				direction *= vel.velocity / glm::length(direction);
				transform.position += direction;
			}
			else {
				if (m_registry.has<projectileType::Damage>(entity)) {
					m_emitter.publish<evnt::EntityDamaged>(targeting.targetId, 1.0f);
					m_registry.destroy(entity);
				}
				if (m_registry.has<projectileType::Slow>(entity)) {
					cmpt::Velocity& vel = m_registry.get<cmpt::Velocity>(targeting.targetId);
					vel.multiplierAge = 0;
					vel.velMultiplier = TOWER_SLOW_AMOUNT;
					vel.multiplierLifespan = SLOW_DURATION;
					m_registry.destroy(entity);
				}
			}
		}
		else {
			m_registry.destroy(entity);
		}
	});
}
