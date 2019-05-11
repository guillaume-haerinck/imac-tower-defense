#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <entt/entt.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"
#include "level-hud-bindings.hpp"

class LevelHud : public Noesis::StackPanel {
public:
	LevelHud(EventEmitter& emitter, Progression& progression, entt::DefaultRegistry& registry);

private:
	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	void OnMouseEnter(const Noesis::MouseEventArgs& e) override;
	void OnMouseLeave(const Noesis::MouseEventArgs& e) override;

private:
	NS_DECLARE_REFLECTION(LevelHud, StackPanel)

	Noesis::Ptr<LevelHudBindings> m_bindings;
	EventEmitter& m_emitter;
	Progression& m_progression;
	entt::DefaultRegistry& m_registry;
};

/*

ConstructionSystem::ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level, Progression& progression)
	: ISystem(registry, emitter), m_level(level), m_towerFactory(registry), m_mirrorFactory(registry), m_progression(progression) {
	m_emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		glm::vec2 tilePosition = this->m_level.projToGrid(event.position.x / WIN_RATIO, event.position.y);
		unsigned int tileId = this->m_level.getTile(tilePosition.x, tilePosition.y);
		this->m_registry.assign<tileTag::Constructible>(tileId);
		this->m_registry.remove<cmpt::EntityOn>(tileId);
	});
}

//Mirror
void ConstructionSystem::onLeftClickDown(const evnt::LeftClickDown& event) {
	//Safety
	removeControlTags();
	//Get tile
	glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
	unsigned int tileId = this->m_level.getTile(tilePosition.x, tilePosition.y);
	if (tileId != -1) {
		//Construct
		if (m_registry.has<tileTag::Constructible>(tileId)) { //&& m_emitter.focus == FocusMode::GAME) {// && m_progression.getMoney() >= MIRROR_COST) {
			cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(tileId);
			unsigned int mirrorId = this->m_mirrorFactory.create(trans.position.x, trans.position.y);
			this->m_registry.remove<tileTag::Constructible>(tileId);
			this->m_registry.assign<cmpt::EntityOn>(tileId, mirrorId);
			m_progression.addToMoney(-MIRROR_COST);
		}
		//Rotate
		if (m_registry.has<cmpt::EntityOn>(tileId)) {
			unsigned int entityId = m_registry.get<cmpt::EntityOn>(tileId).entityId;
			m_emitter.publish<evnt::Selected>(entityId);

			if (m_registry.has<entityTag::Mirror>(entityId)) {
				m_registry.assign<stateTag::IsBeingControlled>(entityId);
				m_registry.assign<cmpt::LookAtMouse>(entityId);
			}
			if (m_registry.has<cmpt::ShootLaser>(entityId)) {
				m_registry.get<cmpt::ShootLaser>(entityId).isActiv = !m_registry.get<cmpt::ShootLaser>(entityId).isActiv;
			}
		}
	}
}

void ConstructionSystem::onLeftClickUp(const evnt::LeftClickUp& event) {
	removeControlTags();
}

//Tower
void ConstructionSystem::onRightClickDown(const evnt::RightClickDown& event) {
	//Safety
	removeControlTags();
	//Get tile
	glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
	unsigned int tileId = this->m_level.getTile(tilePosition.x, tilePosition.y);
	if (tileId != -1) {

		// Construct
		if (m_registry.has<tileTag::Constructible>(tileId)) { // && m_emitter.focus == FocusMode::GAME) {// && m_progression.getMoney() >= TOWER_COST) {
			cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(tileId);
			unsigned int towerId = this->m_towerFactory.create(trans.position.x, trans.position.y);
			this->m_registry.remove<tileTag::Constructible>(tileId);
			this->m_registry.assign<cmpt::EntityOn>(tileId, towerId);
			m_progression.addToMoney(-TOWER_COST);
			m_registry.assign<stateTag::IsBeingControlled>(towerId);
			m_registry.assign<cmpt::LookAtMouse>(towerId);
		}

		//Rotate
		if (m_registry.has<cmpt::EntityOn>(tileId) && m_emitter.focus == FocusMode::GAME) {
			unsigned int entityId = m_registry.get<cmpt::EntityOn>(tileId).entityId;
			m_emitter.publish<evnt::Selected>(entityId);

			if (m_registry.has<entityTag::Mirror>(entityId)) {
				m_registry.assign<stateTag::IsBeingControlled>(entityId);
				m_registry.assign<cmpt::LookAtMouse>(entityId);
			}
			if (m_registry.has<cmpt::ShootLaser>(entityId)) {
				m_registry.get<cmpt::ShootLaser>(entityId).isActiv = !m_registry.get<cmpt::ShootLaser>(entityId).isActiv;
			}
		}
	}
}

void ConstructionSystem::onRightClickUp(const evnt::RightClickUp& event) {
	removeControlTags();
}

void ConstructionSystem::removeControlTags() {
	m_registry.view<stateTag::IsBeingControlled>().each([this](auto entity, auto) {
		m_registry.remove<stateTag::IsBeingControlled>(entity);
		if (m_registry.has<cmpt::LookAtMouse>(entity)) {
			m_registry.remove<cmpt::LookAtMouse>(entity);
		}
		if (m_registry.has<cmpt::ShootLaser>(entity)) {
			m_registry.get<cmpt::ShootLaser>(entity).isActiv = true;
		}
	});
}

void ConstructionSystem::update(float deltatime) {}

*/