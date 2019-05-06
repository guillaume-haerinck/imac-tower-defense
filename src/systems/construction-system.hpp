#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "entity-factories/tower-factory.hpp"
#include "entity-factories/mirror-factory.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/left-click-up.hpp"
#include "events/right-click-up.hpp"
#include "core/level/level.hpp"
#include "core/progression.hpp"

class ConstructionSystem : public ISystem {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level, Progression& progression);
	void connectEvents() override;
	void disconnectEvents() override;
	void update(float deltatime) override;

private:
	EventEmitter& m_emitter;
	Level& m_level;
	Progression& m_progression;
	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_clickUpCon;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>> m_clickRightUpCon;
};
