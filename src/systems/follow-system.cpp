#include "follow-system.hpp"

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include <spdlog/spdlog.h>
#include <math.h> 

#include "core/maths.hpp"
#include "core/constants.hpp"
#include "events/mouse-move.hpp"
#include "components/transform.hpp"
#include "components/look-at.hpp"

FollowSystem::FollowSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: System(registry), m_emitter(emitter)
{
	m_emitter.on<evnt::MouseMove>([this](const evnt::MouseMove& event, EventEmitter& emitter) {
		this->m_mousePos = event.mousePos;
		this->m_mousePos.x *= WIN_RATIO;
	});
}

void FollowSystem::update(double deltatime) {
	m_registry.view<cmpt::Transform, cmpt::LookAt>().each([this, deltatime](auto entity, cmpt::Transform& transform, cmpt::LookAt& lookAt) {
		glm::vec2 direction = transform.position - this->m_mousePos;
		transform.rotation = -atan2(direction.x, direction.y) - M_PI / 2;
	});
}
