#include "follow-system.hpp"

#include <spdlog/spdlog.h>

#include "events/move.hpp"
#include "components/transform.hpp"
#include "components/look-at.hpp"

FollowSystem::FollowSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: System(registry), m_emitter(emitter)
{
	m_emitter.on<evnt::Move>([this](const evnt::Move& event, EventEmitter& emitter) {
		spdlog::info("Mouse pos is at: {} {}", event.mousePos.x, event.mousePos.y);
	});
}

void FollowSystem::update(double deltatime) {
	/*
	cmpt::Transform& transform = m_registry.get<cmpt::Transform>(myEntity);
	transform.position.x = 0;
	*/

	m_registry.view<cmpt::Transform, cmpt::LookAt>().each([this](auto entity, cmpt::Transform & transform, cmpt::LookAt & lookAt) {
		transform.position.x += 2;
	});
}
