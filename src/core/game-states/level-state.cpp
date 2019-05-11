#include "level-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "core/game.hpp"
#include "core/tags.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/entity-on.hpp"
#include "components/look-at-mouse.hpp"

LevelState::LevelState(Game& game)
: IGameState(game), m_levelHud(game.emitter, game.progression), m_state(LevelInteractionState::FREE), m_towerFactory(game.registry), m_mirrorFactory(game.registry)
{
	m_xaml = m_levelHud;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);

	/*
	// TODO handle unselection
	m_emitter.on<evnt::Selected>([this](const evnt::Selected & event, EventEmitter & emitter) {
		if (m_registry.has<entityTag::Tower>(event.entity)) {
			const glm::vec2 position = m_registry.get<cmpt::Transform>(event.entity).position;
			this->bHide = false;

			const float posXWindow = imaths::rangeMapping(position.x, 0.0f, PROJ_WIDTH_RAT, 0.0f, WIN_WIDTH);
			const float posYWindow = imaths::rangeMapping(position.y, 0.0f, PROJ_HEIGHT, 0.0f, WIN_HEIGHT);

			this->setPosX(posXWindow);
			this->setPosY(posYWindow);
		}
	});
	*/
}

LevelState::~LevelState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelState::enter() {
	// Subscribe self to input events
	connectInputs();
}

void LevelState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

	m_game.animationSystem->update(deltatime); 
	m_game.movementSystem->update(deltatime);
	m_game.renderSystem->update(deltatime);
	m_game.attackSystem->update(deltatime);
	m_ui->GetRenderer()->Render();

	switch (checkVictoryConditions()) {
	case GameplayState::LOOSE:
		// change game state to game over screen ? Use a personalized message
		break;

	case GameplayState::WIN:
		// change to next level introduction
		break;

	default:
		break;
	}
}

GameplayState LevelState::checkVictoryConditions() {
	return GameplayState::PLAY;
}

void LevelState::exit() {
	// Remove self from input events
	disconnectInputs();
}

void LevelState::onLeftClickUp(const evnt::LeftClickUp& event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);

	
}

void LevelState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);

	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case FREE: 
		{
			// Get entity. If valid mirror rotate. Else Invalid
			int entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (entityId != -1) {
				if (m_game.registry.has<entityTag::Mirror>(entityId)) {
					m_state = LevelInteractionState::ROTATE;
					m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
					m_game.registry.assign<cmpt::LookAtMouse>(entityId);
				}
			}
			else {
				m_state = LevelInteractionState::INVALID;
			}
			break;
		}

		case ROTATE:
			spdlog::warn("[Level State] Impossible state reached : cannot click down and already be rotating");
			break;

		case INVALID:
			// Stop the invalid animation if click during
			m_state = LevelInteractionState::FREE;
			break;

		case OPTIONS:
			// Click outside option menu closes it
			m_state = LevelInteractionState::FREE;
			break;

		case BUILD:
		{
			// Build selected type on tile if valid
			int tileId = m_game.level->getTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.has<tileTag::Constructible>(tileId)) {
				cmpt::Transform trans = m_game.registry.get<cmpt::Transform>(tileId);
				unsigned int mirrorId = m_mirrorFactory.create(trans.position.x, trans.position.y);

				m_game.registry.remove<tileTag::Constructible>(tileId);
				m_game.registry.assign<cmpt::EntityOn>(tileId, mirrorId);
				m_game.progression.addToMoney(-MIRROR_COST);
			}
			else {
				m_state = LevelInteractionState::INVALID;
			}
			break;
		}

		default:
			break;
		}
	}
}

void LevelState::onMouseMove(const evnt::MouseMove& event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);
}


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