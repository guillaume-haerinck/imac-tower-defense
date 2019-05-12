#include "level-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "core/game.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "events/interactions/select-rotation.hpp"
#include "events/tower-dead.hpp"
#include "events/interactions/delete-entity.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/entity-on.hpp"
#include "components/look-at-mouse.hpp"

LevelState::LevelState(Game& game)
	: IGameState(game), m_levelHud(game.emitter, game.progression), m_state(LevelInteractionState::FREE),
	m_towerFactory(game.registry), m_mirrorFactory(game.registry), m_invalidTimeCounter(0), m_invalidTimeMax(1 * 60)
{
	m_xaml = m_levelHud;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);

	game.emitter.on<evnt::ConstructSelection>([this](const evnt::ConstructSelection & event, EventEmitter & emitter) {
		this->m_constructType = event.type;
		this->changeState(LevelInteractionState::BUILD);
	});

	game.emitter.on<evnt::DeleteEntity>([this](const evnt::DeleteEntity & event, EventEmitter & emitter) {
		spdlog::info("Asked to delete entity");
	});

	game.emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		glm::vec2 tilePosition = this->m_game.level->projToGrid(event.position.x / WIN_RATIO, event.position.y);
		unsigned int tileId = this->m_game.level->getTile(tilePosition.x, tilePosition.y);
		this->m_game.registry.assign<tileTag::Constructible>(tileId);
		this->m_game.registry.remove<cmpt::EntityOn>(tileId);
	});

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

/* ----------------------- GETTERS ------------------------ */

LevelInteractionState LevelState::getInteractionState() const {
	return m_state;
}

/* ------------------------ SETTERS ------------------------- */
void LevelState::changeState(LevelInteractionState state) {
	// Exit current state
	switch (m_state) {
	case FREE:
		break;

	case ROTATE:
		m_game.registry.remove<stateTag::IsBeingControlled>(m_lastSelectedEntity);
		m_game.registry.remove<cmpt::LookAtMouse>(m_lastSelectedEntity);
		break;

	case INVALID:
		break;

	case OPTIONS:
		m_levelHud.setOptionsVisibilityTo(false);
		break;

	case BUILD:
		break;
	default:
		break;
	}

	// Enter new state
	switch (state) {
	case FREE:
		break;
	case ROTATE:
		break;
	case INVALID:
		break;
	case OPTIONS:
		break;
	case BUILD:
		break;
	default:
		break;
	}

	// Change state
	m_state = state;
}

/* ------------------------ STATE MACHINE ------------------------ */

void LevelState::enter() {
	// Subscribe self to input events
	connectInputs();
}

void LevelState::update(float deltatime) {
	if (m_state == LevelInteractionState::INVALID) {
		m_invalidTimeCounter++;
	}

	if (m_invalidTimeCounter >= m_invalidTimeMax) {
		m_invalidTimeCounter = 0;
		changeState(LevelInteractionState::FREE);
	}

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

/* ----------------------------------- INPUT EVENTS --------------------------- */

void LevelState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);

	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case FREE:
		{
			// Get entity. If valid mirror rotate. Else Invalid
			int entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.valid(entityId)) {
				if (m_game.registry.has<entityTag::Mirror>(entityId) || m_game.registry.has<entityTag::Tower>(entityId)) {
					changeState(LevelInteractionState::ROTATE);
					m_game.registry.accommodate<stateTag::IsBeingControlled>(entityId);
					m_game.registry.accommodate<cmpt::LookAtMouse>(entityId);

					m_lastSelectedEntity = entityId;
				}
			}
			else {
				spdlog::warn("No valid entity on tile");
				changeState(LevelInteractionState::INVALID);
			}
			break;
		}

		case ROTATE:
			break;

		case INVALID:
			// Stop the invalid animation if click during
			changeState(LevelInteractionState::FREE);
			break;

		case OPTIONS:
			// Click outside option menu closes it
			changeState(LevelInteractionState::FREE);
			break;

		case BUILD:
		{
			// Build selected type on tile if valid
			int tileId = m_game.level->getTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.valid(tileId)) {
				if (m_game.registry.has<tileTag::Constructible>(tileId)) {
					cmpt::Transform trans = m_game.registry.get<cmpt::Transform>(tileId);
					unsigned int entityId = 0;

					switch (m_constructType) {
					case MIRROR_BASIC:
						entityId = m_mirrorFactory.create(trans.position.x, trans.position.y);
						m_game.progression.addToMoney(-MIRROR_COST);
						break;

					case TOWER_BASIC:
						entityId = m_towerFactory.create(trans.position.x, trans.position.y);
						m_game.progression.addToMoney(-TOWER_COST);
						break;

					default:
						break;
					}

					m_game.registry.remove<tileTag::Constructible>(tileId);
					m_game.registry.assign<cmpt::EntityOn>(tileId, entityId);

					// Rotatable on build
					m_game.registry.accommodate<stateTag::IsBeingControlled>(entityId);
					m_game.registry.accommodate<cmpt::LookAtMouse>(entityId);
					changeState(LevelInteractionState::ROTATE);
					m_lastSelectedEntity = entityId;
				}
				else {
					spdlog::warn("Not a constructible tile");
					changeState(LevelInteractionState::INVALID);
				}
			}
			else {
				spdlog::warn("Invalid tile");
				changeState(LevelInteractionState::INVALID);
			}
			break;
		}

		default:
			break;
		}
	}
}

void LevelState::onLeftClickUp(const evnt::LeftClickUp& event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);

	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case FREE:
			break;

		case ROTATE:
			// Stop rotating when mouse not pressed
			changeState(LevelInteractionState::FREE);
			break;

		case INVALID:
			break;

		case OPTIONS:
			break;

		case BUILD:
			break;

		default:
			break;
		}
	}
}

void LevelState::onRightClickDown(const evnt::RightClickDown& event) {
	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case FREE:
		case OPTIONS:
		{
			// Get entity. If valid open options. Else Invalid
			int entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (entityId != -1) {
				changeState(LevelInteractionState::OPTIONS);
				cmpt::Transform trans = m_game.registry.get<cmpt::Transform>(entityId);
				glm::vec2 posWindow = glm::vec2(
					imaths::rangeMapping(trans.position.x, 0.0f, PROJ_WIDTH_RAT, 0.0f, WIN_WIDTH),
					imaths::rangeMapping(trans.position.y, 0.0f, PROJ_HEIGHT, 0.0f, WIN_HEIGHT)
				);

				if (m_game.registry.has<entityTag::Mirror>(entityId)) {
					m_levelHud.setOptionsPosition(posWindow);
				} else if (m_game.registry.has<entityTag::Tower>(entityId)) {
					m_levelHud.setOptionsPosition(posWindow);
				}
			}
			else {
				spdlog::warn("No valid entity on tile");
				changeState(LevelInteractionState::INVALID);
			}
			break;
		}

		case ROTATE:
			break;

		case INVALID:
			// Stop the invalid animation if click during
			changeState(LevelInteractionState::FREE);
			break;

		case BUILD:
			break;

		default:
			break;
		}
	}
}

void LevelState::onMouseMove(const evnt::MouseMove& event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);

	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case FREE:
			// TODO move camera if close to screen
			break;

		case ROTATE:
			m_game.emitter.publish<evnt::SelectRotation>(event.mousePos);
			break;

		case INVALID:
			break;

		case OPTIONS:
			break;

		case BUILD:
			// TODO follow mouse with transparent entity to be built
			break;
		default:
			break;
		}
	}
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