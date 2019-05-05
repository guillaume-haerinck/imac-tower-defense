#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "entity-factories/tower-factory.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/left-click-up.hpp"
#include "core/level/level.hpp"

class ConstructionSystem : public ISystem {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level);
	void connectEvents() override;
	void disconnectEvents() override;
	void update(float deltatime) override;

private:
	EventEmitter& m_emitter;
	Level& m_level;
	TowerFactory m_towerFactory;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_clickUpCon;
};
