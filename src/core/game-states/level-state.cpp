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
#include "events/interactions/change-cursor.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/entity-on.hpp"
#include "components/look-at-mouse.hpp"
#include "components/transform.hpp"
#include "components/tint-colour.hpp"
#include "components/shoot-laser.hpp"
#include "components/constrained-rotation.hpp"
#include "components/animated.hpp"
#include "components/animation-alpha.hpp"

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
		case INVALID :
		case FREE : 
			m_game.emitter.entityBeingPlaced = true;
			this->m_constructType = event.type;
			this->changeState(LevelInteractionState::BUILD);
			unsigned int entityId;

			switch (m_constructType) {
			case MIRROR_BASIC:
				entityId = m_mirrorFactory.create(0, 0);
				m_game.registry.assign<positionTag::IsOnHoveredTile>(entityId);
				m_game.progression.addToMoney(-MIRROR_COST);
				break;

			case TOWER_LASER:
				entityId = m_towerFactory.createLaser(0, 0);
				m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
				m_game.registry.assign<positionTag::IsOnHoveredTile>(entityId);
				m_game.registry.assign<stateTag::RotateableByMouse>(entityId);
				m_game.registry.get<cmpt::ShootLaser>(entityId).isActiv = false;
				m_game.progression.addToMoney(-TOWER_LASER_COST);
				break;

			case TOWER_SLOW:
				entityId = m_towerFactory.createSlow(0, 0);
				m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
				m_game.registry.assign<positionTag::IsOnHoveredTile>(entityId);
				m_game.progression.addToMoney(-TOWER_SLOW_COST);
				break;
			}
			m_lastSelectedEntity = entityId;
			break;
		}
	});

	// TODO use a safer and more global way, because if tile is invalid, it will cause a problem
	// Start by using only one type of event for entity deletion, and see if there is a way to check the grid for deletion
	game.emitter.on<evnt::DeleteEntity>([this](const evnt::DeleteEntity & event, EventEmitter & emitter) {
		if (this->m_game.registry.valid(event.entityId)) {
			glm::vec2 position = this->m_game.registry.get<cmpt::Transform>(event.entityId).position;
			//this->m_game.registry.destroy(event.entityId);
			this->m_game.registry.assign<cmpt::Animated>(event.entityId,1,true);
			this->m_game.registry.assign<cmpt::AnimationAlpha>(event.entityId,false);

			std::uint32_t tileId = this->m_game.level->getTileFromProjCoord(position.x / WIN_RATIO, position.y);
			this->m_game.registry.assign<tileTag::Constructible>(tileId);
			this->m_game.registry.remove<cmpt::EntityOnTile>(tileId);

			this->changeState(LevelInteractionState::FREE);
		}
	});

	game.emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		std::uint32_t tileId = this->m_game.level->getTileFromProjCoord(event.position.x / WIN_RATIO, event.position.y);
		this->m_game.registry.accommodate<tileTag::Constructible>(tileId);
		this->m_game.registry.reset<cmpt::EntityOnTile>(tileId);
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
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case INVALID:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::NO);
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case ROTATE:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
		break;

	case OPTIONS:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		break;

	case BUILD:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
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
	m_ui->Update(SDL_GetTicks() / 1000.0f);
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

	//Updates
	m_game.animationSystem->update(deltatime); 
	m_game.movementSystem->update(deltatime);
	m_game.renderSystem->update(deltatime);
	m_game.attackSystem->update(deltatime);
	m_game.lifeAndDeathSystem->update(deltatime);
	m_game.waveSystem->update(deltatime);
	m_ui->GetRenderer()->Render();
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
		case INVALID:
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
					m_game.registry.assign<cmpt::EntityOnTile>(tileId, m_lastSelectedEntity);

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
	std::uint32_t entity;
	if (m_game.registry.valid(m_lastSelectedEntity)) {
		entity = m_lastSelectedEntity;
	}
	else {
		glm::vec2 mousePos = m_game.emitter.mousePos;
		entity = m_game.level->getEntityOnTileFromProjCoord(mousePos.x, mousePos.y);
	}
	if (m_game.registry.valid(entity) && m_game.registry.has<stateTag::RotateableByMouse>(entity)) {
		if (m_game.registry.has<cmpt::ConstrainedRotation>(entity)) {
			cmpt::ConstrainedRotation& constRot = m_game.registry.get<cmpt::ConstrainedRotation>(entity);
			constRot.angleIndex = (constRot.angleIndex +event.value+ constRot.nbAngles) % constRot.nbAngles;
			// Rotate
			m_game.registry.get<cmpt::Transform>(entity).rotation += event.value*constRot.angleStep;
			// Update sprite
			if (m_game.registry.has<cmpt::SpriteAnimation>(entity)) {
				cmpt::SpriteAnimation& spriteAnim = m_game.registry.get<cmpt::SpriteAnimation>(entity);
				spriteAnim.activeTile = constRot.angleIndex;
				spriteAnim.startTile = constRot.angleIndex;
				spriteAnim.endTile = constRot.angleIndex;
			}
		}
		else {
			m_game.registry.get<cmpt::Transform>(entity).rotation += imaths::TAU/32*event.value;
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
			if (m_game.registry.has<entityTag::Tower>(m_lastSelectedEntity)) {
				m_game.registry.reset<stateTag::RotateableByMouse>(m_lastSelectedEntity);
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
		case INVALID:
		case OPTIONS:
		{
			// Get entity. If valid open options. Else Invalid
			std::uint32_t entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.valid(entityId)) {
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
		{
			std::uint32_t entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.valid(entityId)) {
				if (m_game.registry.has<entityTag::Mirror>(entityId)) {
					m_emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
				} else if (m_game.registry.has<entityTag::Tower>(entityId)) {
					// TODO handle activated and desactivated towers
					m_emitter.publish<evnt::ChangeCursor>(CursorType::ACTIVATE);
				}
			} else {
				m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
			}
			break;
		}

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
