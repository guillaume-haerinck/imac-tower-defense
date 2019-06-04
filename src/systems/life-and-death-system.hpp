#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "core/progression.hpp"
#include "events/handlers/event-emitter.hpp"
#include "entity-factories/vfx-factory.hpp"

class LifeAndDeathSystem : public ISystem {
public:
	LifeAndDeathSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression);
	void update(float deltatime) override;

private:
	Progression& m_progression;
	VFXFactory m_vfxFactory;
};
