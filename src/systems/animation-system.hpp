#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/enemy-dead.hpp"

#include "entity-factories/explosion-factory.hpp"

class AnimationSystem : public ISystem {
public:
    AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(float deltatime) override;

private:
	ExplosionFactory m_explosionFactory;
};
