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
#include "components/transform.hpp"
#include "components/tint-colour.hpp"
#include "components/shoot-laser.hpp"
#include "components/constrained-rotation.hpp"

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
		switch (m_state) {
		case FREE : 
			m_game.emitter.entityBeingPlaced = true;
			this->m_constructType = event.type;
			this->changeState(LevelInteractionState::BUILD);
			unsigned int entityId;
			switch (m_constructType) {
			case MIRROR_BASIC:
				entityId = m_mirrorFactory.create(0, 0);
				m_game.progression.addToMoney(-MIRROR_COST);
				break;

			case TOWER_LASER:
				entityId = m_towerFactory.createLaser(0, 0);
				m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
				m_game.registry.get<cmpt::ShootLaser>(entityId).isActiv = false;
				m_game.progression.addToMoney(-TOWER_LASER_COST);
				break;

			case TOWER_SLOW:
				entityId = m_towerFactory.createSlow(0, 0);
				m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
				m_game.progression.addToMoney(-TOWER_SLOW_COST);
				break;

			default:
				break;
			}
			m_lastSelectedEntity = entityId;
		break;
		default :
		break;
		}
	});

	// TODO use a safer and more global way, because if tile is invalid, it will cause a problem
	// Start by using only one type of event for entity deletion, and see if there is a way to check the grid for deletion
	game.emitter.on<evnt::DeleteEntity>([this](const evnt::DeleteEntity & event, EventEmitter & emitter) {
		if (this->m_game.registry.valid(event.entityId)) {
			glm::vec2 position = this->m_game.registry.get<cmpt::Transform>(event.entityId).position;
			this->m_game.registry.destroy(event.entityId);

			std::uint32_t tileId = this->m_game.level->getTileFromProjCoord(position.x / WIN_RATIO, position.y);
			this->m_game.registry.assign<tileTag::Constructible>(tileId);
			this->m_game.registry.remove<cmpt::EntityOn>(tileId);

			this->changeState(LevelInteractionState::FREE);
		}
	});

	game.emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		std::uint32_t tileId = this->m_game.level->getTileFromProjCoord(event.position.x / WIN_RATIO, event.position.y);
		this->m_game.registry.assign<tileTag::Constructible>(tileId);
		this->m_game.registry.remove<cmpt::EntityOn>(tileId);
	});
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
	case INVALID:
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case ROTATE:
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

	//Updates
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
			// Get entity. If valid mirror rotate. If laser then switch on or off. Else invalid
			int entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.valid(entityId)) {
				if (m_game.registry.has<entityTag::Mirror>(entityId)) {
					changeState(LevelInteractionState::ROTATE);
					m_game.registry.accommodate<stateTag::IsBeingControlled>(entityId);
					m_game.registry.accommodate<cmpt::LookAtMouse>(entityId);

					m_lastSelectedEntity = entityId;
					m_levelHud.setSelectedEntity(entityId);
				}
				if (m_game.registry.has<cmpt::ShootLaser>(entityId)) {
					cmpt::ShootLaser& shootLaser = m_game.registry.get<cmpt::ShootLaser>(entityId);
					shootLaser.isActiv = !shootLaser.isActiv;
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
					m_game.emitter.entityBeingPlaced = false;
					cmpt::Transform trans = m_game.registry.get<cmpt::Transform>(tileId);

					m_game.registry.remove<tileTag::Constructible>(tileId);
					m_game.registry.assign<cmpt::EntityOn>(tileId, m_lastSelectedEntity);

					m_game.registry.remove<positionTag::IsOnHoveredTile>(m_lastSelectedEntity);
					m_game.registry.reset<stateTag::IsBeingControlled>(m_lastSelectedEntity);
					m_game.registry.get<cmpt::Transform>(m_lastSelectedEntity).position += trans.position;
					if (m_game.registry.has<cmpt::ShootLaser>(m_lastSelectedEntity)) {
						m_game.registry.get<cmpt::ShootLaser>(m_lastSelectedEntity).isActiv = false;
					}

					// Rotatable on build
					if (m_game.registry.has<stateTag::RotateableByMouse>(m_lastSelectedEntity)) {
						m_game.registry.accommodate<stateTag::IsBeingControlled>(m_lastSelectedEntity);
						m_game.registry.accommodate<cmpt::LookAtMouse>(m_lastSelectedEntity);
						changeState(LevelInteractionState::ROTATE);
					}
					else {
						changeState(LevelInteractionState::FREE);
					}
				}
				else {
					spdlog::warn("Not a constructible tile");
					//changeState(LevelInteractionState::INVALID);
				}
			}
			else {
				spdlog::warn("Invalid tile");
				//changeState(LevelInteractionState::INVALID);
			}
			break;
		}

		default:
			break;
		}
	}
}

void LevelState::onMouseScrolled(const evnt::MouseScrolled& event) {
	if (m_game.registry.valid(m_lastSelectedEntity) && m_game.registry.has<stateTag::RotateableByMouse>(m_lastSelectedEntity)) {
		if (m_game.registry.has<cmpt::ConstrainedRotation>(m_lastSelectedEntity)) {
			cmpt::ConstrainedRotation& constRot = m_game.registry.get<cmpt::ConstrainedRotation>(m_lastSelectedEntity);
			m_game.registry.get<cmpt::Transform>(m_lastSelectedEntity).rotation += constRot.angleStep*event.value;
		}
		else {
			m_game.registry.get<cmpt::Transform>(m_lastSelectedEntity).rotation += imaths::TAU/32*event.value;
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
			if (m_game.registry.has<cmpt::ShootLaser>(m_lastSelectedEntity)) {
				m_game.registry.get<cmpt::ShootLaser>(m_lastSelectedEntity) = true;
			}
			m_lastSelectedEntity = entt::null;
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
				m_levelHud.setSelectedEntity(entityId);
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
	m_game.emitter.mousePos = event.mousePos;

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
