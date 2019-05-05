#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/projectile-hit.hpp"

class HealthSystem : public ISystem {
public:
	HealthSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
	void update(float deltatime) override;
	void connectEvents() override;
	void disconnectEvents() override;

private:
	EventEmitter& m_emitter;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::ProjectileHit>> m_projectileCon;
};
