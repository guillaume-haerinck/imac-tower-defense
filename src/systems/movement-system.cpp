#include "movement-system.hpp"

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include <spdlog/spdlog.h>
#include <math.h> 

#include "core/maths.hpp"
#include "core/constants.hpp"
#include "events/mouse-move.hpp"
#include "components/transform.hpp"
#include "components/rigid-body.hpp"
#include "components/look-at.hpp"
#include "components/trajectory.hpp"

MovementSystem::MovementSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: System(registry), m_emitter(emitter)
{
	m_emitter.on<evnt::MouseMove>([this](const evnt::MouseMove& event, EventEmitter& emitter) {
		this->m_mousePos = event.mousePos;
		this->m_mousePos.x *= WIN_RATIO;
	});
}

void MovementSystem::update(double deltatime) {
	m_registry.view<cmpt::RigidBody, cmpt::Transform>().each([](auto entity, cmpt::RigidBody & rigidbody, cmpt::Transform & transform) {
		transform.position.x = rigidbody.body->GetPosition().x;
		transform.position.y = rigidbody.body->GetPosition().y;
		transform.rotation = rigidbody.body->GetAngle();
	});

	m_registry.view<cmpt::Transform, cmpt::LookAt>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::LookAt& lookAt) {
		glm::vec2 direction =  this->m_mousePos - transform.position;
		transform.rotation = atan2(direction.y, direction.x) ;
	});

	m_registry.view<cmpt::Transform, cmpt::Trajectory>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::Trajectory& traj) {
		glm::vec2 direction = traj.traj.at(traj.currentTarget) - transform.position;
		float norm = glm::length(direction);
		if (norm > 1) {
			direction *= 0.5 / glm::length(direction);
			transform.position += direction;
		}
		else if( traj.currentTarget < traj.traj.size()-1){
			traj.currentTarget++;
		}
	});
}
