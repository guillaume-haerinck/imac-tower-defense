#pragma once

#include <entt/entt.hpp>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"

#include "entity-factories/explosion-factory.hpp"

class AnimationSystem : public System {
public:
    AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(double deltatime);

private:
	EventEmitter & m_emitter;
	ExplosionFactory m_explosionFactory;
};
