#pragma once

#include "system.hpp"
#include "events/event-emitter.hpp"

class FollowSystem : public System {
public:
	FollowSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(double deltatime);

private:
	EventEmitter& m_emitter;
};
