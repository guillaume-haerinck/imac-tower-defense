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
#include "components/transform.hpp"
#include "components/rigid-body.hpp"
#include "components/look-at.hpp"
#include "components/look-at-mouse.hpp"
#include "components/trajectory.hpp"
#include "components/follow.hpp"
#include "components/pathfinding.hpp"
#include "components/targeting.hpp"


MovementSystem::MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: ISystem(registry, emitter) {}

void MovementSystem::onMouseMove(const evnt::MouseMove& event) {
	m_registry.view<cmpt::Transform, cmpt::LookAtMouse>().each([this, event](auto entity, cmpt::Transform & transform, cmpt::LookAtMouse& lookAtMouse) {
		//float agl = atan2(event.mousePos.y - transform.position.y, event.mousePos.x * WIN_RATIO - transform.position.x);
		glm::vec2 toPrevMouse = m_prevMousePos*glm::vec2(WIN_RATIO,1.0f) - transform.position;
		glm::vec2 toMouse = event.mousePos*glm::vec2(WIN_RATIO, 1.0f) - transform.position;
		float deltaAgl = atan2(toPrevMouse.y, toPrevMouse.x) - atan2(toMouse.y, toMouse.x);
		transform.rotation -= deltaAgl;//+ lookAtMouse.angleOffset;
	});
	m_prevMousePos = event.mousePos;
}

void MovementSystem::update(float deltatime) {
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

	m_registry.view<cmpt::Transform, cmpt::Pathfinding>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Pathfinding& pathfinding) {
		Level* level = pathfinding.level;
		glm::vec2 direction = level->getNodePosition(pathfinding.currentTarget) - transform.position;
		float norm = glm::length(direction);
		if (norm > 1) {
		direction *= 0.5 / glm::length(direction);
		transform.position += direction;
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
			if (norm > 1) {
				direction *= follow.velocity / glm::length(direction);
				transform.position += direction;
			}
			else {
				m_emitter.publish<evnt::EnemyDamaged>(targeting.targetId, targetPosition,1.0f);
				m_registry.destroy(entity);
			}
		}
		else {
			m_registry.destroy(entity);
		}
	});
}
