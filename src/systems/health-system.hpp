#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "core/progression.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/enemy-damaged.hpp"

class HealthSystem : public ISystem {
public:
	HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression);
	void update(float deltatime) override;

private:
	Progression& m_progression;
};
