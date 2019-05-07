#include "construction-system.hpp"

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "components/transform.hpp"
#include "events/tower-dead.hpp"

ConstructionSystem::ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level, Progression& progression)
: ISystem(registry, emitter), m_level(level), m_towerFactory(registry), m_mirrorFactory(registry), m_progression(progression) {
	m_emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		glm::vec2 tilePosition = this->m_level.projToGrid(event.position.x/WIN_RATIO, event.position.y);
		unsigned int entityId = this->m_level.getTile(tilePosition.x, tilePosition.y);
		this->m_registry.assign<tileTag::Constructible>(entityId);
	});
}

void ConstructionSystem::onLeftClickUp(const evnt::LeftClickUp& event) {
	glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
	unsigned int entityId = this->m_level.getTile(tilePosition.x, tilePosition.y);
	if (entityId != -1) {
		if (this->m_registry.has<tileTag::Constructible>(entityId)) {//&& m_progression.getMoney() >= TOWER_COST) {
			cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(entityId);
			this->m_towerFactory.create(trans.position.x, trans.position.y);
			this->m_registry.remove<tileTag::Constructible>(entityId);
			m_progression.addToMoney(-TOWER_COST);
		}
	}
}

void ConstructionSystem::onRightClickUp(const evnt::RightClickUp& event) {
	glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
	unsigned int entityId = this->m_level.getTile(tilePosition.x, tilePosition.y);
	if (entityId != -1) {
		if (this->m_registry.has<tileTag::Constructible>(entityId)) {// && m_progression.getMoney() >= 10) {
			cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(entityId);
			this->m_mirrorFactory.create(trans.position.x, trans.position.y);
			this->m_registry.remove<tileTag::Constructible>(entityId);
			m_progression.addToMoney(-10);
		}
	}
}

void ConstructionSystem::update(float deltatime) {}
