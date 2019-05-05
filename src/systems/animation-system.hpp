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
	void connectEvents() override;
	void disconnectEvents() override;

private:
	EventEmitter & m_emitter;
	ExplosionFactory m_explosionFactory;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::EnnemyDead>> m_enemyDeadCon;
};
