#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"

#include "entity-factories/vfx-factory.hpp"

class AnimationSystem : public ISystem {
public:
    AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(float deltatime) override;

private:
	VFXFactory m_vfxFactory;
};
