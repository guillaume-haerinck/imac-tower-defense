#include "construction-system.hpp"

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "components/transform.hpp"

ConstructionSystem::ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level, Progression& progression)
: ISystem(registry), m_emitter(emitter), m_level(level), m_towerFactory(registry), m_mirrorFactory(registry), m_progression(progression) {}

void ConstructionSystem::connectEvents() {
	if (m_bConnected == false) {
		auto connection = m_emitter.on<evnt::LeftClickUp>([this](const evnt::LeftClickUp & event, EventEmitter & emitter) {
			glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
			unsigned int entityId = this->m_level.getTile(tilePosition.x, tilePosition.y);
			if (entityId != -1) {
				if (this->m_registry.has<tileTag::Constructible>(entityId) ) {//&& m_progression.getMoney() >= 10) {
					cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(entityId);
					this->m_towerFactory.create(trans.position.x, trans.position.y);
					this->m_registry.remove<tileTag::Constructible>(entityId);
					m_progression.addToMoney(-10);
				}
			}
			});
		m_clickUpCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>>(connection);

		//Right clic

		auto connection2 = m_emitter.on<evnt::RightClickUp>([this](const evnt::RightClickUp & event, EventEmitter & emitter) {
			glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
			unsigned int entityId = this->m_level.getTile(tilePosition.x, tilePosition.y);
			if (entityId != -1) {
				if (this->m_registry.has<tileTag::Constructible>(entityId) ){// && m_progression.getMoney() >= 10) {
					cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(entityId);
					this->m_mirrorFactory.create(trans.position.x, trans.position.y);
					this->m_registry.remove<tileTag::Constructible>(entityId);
					m_progression.addToMoney(-10);
				}
			}
		});
		m_clickRightUpCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>>(connection2);

		//

		m_bConnected = true;
	}
}

void ConstructionSystem::disconnectEvents() {
	if (m_bConnected == true) {
		m_emitter.erase(*m_clickUpCon);
		m_clickUpCon.reset();

		//Mirror
		m_emitter.erase(*m_clickRightUpCon);
		m_clickRightUpCon.reset();

		m_bConnected = false;
	}
}

void ConstructionSystem::update(float deltatime) {

}
