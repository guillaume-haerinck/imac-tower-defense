#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "i-system.hpp"
#include "entity-factories/tower-factory.hpp"
#include "entity-factories/mirror-factory.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/left-click-down.hpp"
#include "events/left-click-up.hpp"
#include "events/right-click-down.hpp"
#include "events/right-click-up.hpp"
#include "core/level/level.hpp"
#include "core/progression.hpp"

class ConstructionSystem : public ISystem {
public:
	ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level, Progression& progression);
	void update(float deltatime) override;
	void onLeftClickDown(const evnt::LeftClickDown& event) override;
	void onLeftClickUp(const evnt::LeftClickUp& event) override;
	void onRightClickDown(const evnt::RightClickDown& event) override;
	void onRightClickUp(const evnt::RightClickUp& event) override;

private:
	unsigned int m_currentEntity;
	Level& m_level;
	Progression& m_progression;
	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;
};
