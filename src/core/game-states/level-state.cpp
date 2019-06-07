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
#include "events/wave-updated.hpp"
#include "events/loose.hpp"
#include "events/victory-delay-ends.hpp"
#include "events/enemy-dead.hpp"
#include "events/change-game-state.hpp"
#include "events/enemy-reached-end.hpp"
#include "events/progression-updated.hpp"
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
#include "components/age.hpp"


LevelState::LevelState(Game& game)
	: IGameState(game), m_levelHud(game.emitter, game.progression), m_state(LevelInteractionState::FREE),
	m_towerFactory(game.registry), m_mirrorFactory(game.registry), m_invalidTimeCounter(0), m_invalidTimeMax(1 * 60)
{
	m_xaml = m_levelHud;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);

	handleVictoryConditions();
	handleConstructions();
}

LevelState::~LevelState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

/* ------------- EVENT HANDLING --------------- */

void LevelState::handleVictoryConditions() {
	m_emitter.on<evnt::WaveUpdated>([this](const evnt::WaveUpdated & event, EventEmitter & emitter) {
		switch (event.state) {
		case WaveState::NOT_STARTED:
		case WaveState::PENDING:
		case WaveState::DURING:
			this->m_bWaveDone = false;
			break;

		case WaveState::DONE:
			this->m_bWaveDone = true;
			break;

		default:
			break;
		}
	});

	m_emitter.on<evnt::EnemyDead>([this](const evnt::EnemyDead & event, EventEmitter & emitter) {
		if (this->m_bWaveDone) {
			int enemyRemaining = 0;
			this->m_game.registry.view<entityTag::Enemy>().each([this, &enemyRemaining](auto entity, auto) {
				if (!this->m_game.registry.has<stateTag::IsDisappearing>(entity)) {
					enemyRemaining++;
				}
			});
			if (enemyRemaining == 1) {
				//Set delay before victory and changing game state
				std::uint32_t timer = this->m_game.registry.create();
				this->m_game.registry.assign<cmpt::Age>(timer,DELAY_BETWEEN_VICTORY_AND_CHANGE_GAME_STATE);
				this->m_game.registry.assign<entityTag::VictoryDelayTimer>(timer);
			}
		}
	});

	m_emitter.on<evnt::VictoryDelayEnds>([this](const evnt::VictoryDelayEnds & event, EventEmitter & emitter) {
		this->m_game.emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_EXIT, this->m_game.progression.getLevelNumber());
	});

	m_emitter.on<evnt::EnemyReachedEnd>([this](const evnt::EnemyReachedEnd & event, EventEmitter & emitter) {
		if (this->m_bWaveDone) {
			int enemyRemaining = 0;
			this->m_game.registry.view<entityTag::Enemy>().each([this, &enemyRemaining](auto entity, auto) {
				if (!this->m_game.registry.has<stateTag::IsDisappearing>(entity)) {
					enemyRemaining++;
				}
			});
			if (enemyRemaining == 0) {
				//Set delay before victory and changing game state
				std::uint32_t timer = this->m_game.registry.create();
				this->m_game.registry.assign<cmpt::Age>(timer, DELAY_BETWEEN_VICTORY_AND_CHANGE_GAME_STATE);
				this->m_game.registry.assign<entityTag::VictoryDelayTimer>(timer);
			}
		}
	});

	m_emitter.on<evnt::Loose>([this](const evnt::Loose & event, EventEmitter & emitter) {
		// TODO play an outro
		this->m_game.emitter.publish<evnt::ChangeGameState>(GameState::GAME_OVER);
	});
}

void LevelState::handleConstructions() {
	m_game.emitter.on<evnt::ConstructSelection>([this](const evnt::ConstructSelection & event, EventEmitter & emitter) {
		switch (m_state) {
		case LevelInteractionState::INVALID:
		case LevelInteractionState::FREE:
			m_game.emitter.entityBeingPlaced = true;
			this->m_constructType = event.type;
			this->changeState(LevelInteractionState::BUILD);
			unsigned int entityId;

			switch (m_constructType) {
			case ConstructibleType::MIRROR_BASIC:
				entityId = m_mirrorFactory.create(0, 0);
				m_game.registry.assign<positionTag::IsOnHoveredTile>(entityId);
				m_game.progression.reduceMirrorNumberBy1();
				break;

			case ConstructibleType::TOWER_LASER:
				entityId = m_towerFactory.createLaser(0, 0);
				m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
				m_game.registry.assign<positionTag::IsOnHoveredTile>(entityId);
				m_game.registry.assign<stateTag::RotateableByMouse>(entityId);
				m_game.registry.get<cmpt::ShootLaser>(entityId).isActiv = false;
				break;

			case ConstructibleType::TOWER_SLOW:
				entityId = m_towerFactory.createSlow(0, 0);
				m_game.registry.assign<stateTag::IsBeingControlled>(entityId);
				m_game.registry.assign<positionTag::IsOnHoveredTile>(entityId);
				m_game.progression.reduceSlowNumberBy1();
				break;
			}
			m_lastSelectedEntity = entityId;
			break;
		}
	});

	// TODO use a safer and more global way, because if tile is invalid, it will cause a problem
	// Start by using only one type of event for entity deletion, and see if there is a way to check the grid for deletion
	m_game.emitter.on<evnt::DeleteEntity>([this](const evnt::DeleteEntity & event, EventEmitter & emitter) {
		if (this->m_game.registry.valid(event.entityId)) {
			glm::vec2 position = this->m_game.registry.get<cmpt::Transform>(event.entityId).position;
			//this->m_game.registry.destroy(event.entityId);
			this->m_game.registry.assign<cmpt::Animated>(event.entityId, 1, true);
			this->m_game.registry.assign<cmpt::AnimationAlpha>(event.entityId, false);

			if (this->m_game.registry.has<entityTag::Mirror>(event.entityId)) {
				this->m_game.progression.increaseMirrorNumberBy1();
			}
			if (this->m_game.registry.has<entityTag::Tower>(event.entityId)) {
				this->m_game.progression.increaseSlowNumberBy1();
			}

			std::uint32_t tileId = this->m_game.level->getTileFromProjCoord(position.x / WIN_RATIO, position.y);
			this->m_game.registry.assign<tileTag::Constructible>(tileId);
			this->m_game.registry.remove<cmpt::EntityOnTile>(tileId);

			this->changeState(LevelInteractionState::FREE);
		}
	});

	m_game.emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		std::uint32_t tileId = this->m_game.level->getTileFromProjCoord(event.position.x / WIN_RATIO, event.position.y);
		this->m_game.registry.accommodate<tileTag::Constructible>(tileId);
		this->m_game.registry.reset<cmpt::EntityOnTile>(tileId);
	});
}

/* ----------------------- GETTERS ------------------------ */

LevelInteractionState LevelState::getInteractionState() const {
	return m_state;
}

/* ------------------------ SETTERS ------------------------- */

void LevelState::changeState(LevelInteractionState state) {
	// Exit current state
	switch (m_state) {
	case LevelInteractionState::FREE:
		break;

	case LevelInteractionState::ROTATE:
		m_game.registry.remove<stateTag::IsBeingControlled>(m_lastSelectedEntity);
		m_game.registry.remove<cmpt::LookAtMouse>(m_lastSelectedEntity);
		break;

	case LevelInteractionState::INVALID:
		break;

	case LevelInteractionState::OPTIONS:
		m_levelHud.setOptionsVisibilityTo(false);
		break;

	case LevelInteractionState::BUILD:
		break;
	default:
		break;
	}

	// Enter new state
	switch (state) {
	case LevelInteractionState::FREE:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case LevelInteractionState::INVALID:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::NO);
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case LevelInteractionState::ROTATE:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
		break;

	case LevelInteractionState::OPTIONS:
		m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		break;

	case LevelInteractionState::BUILD:
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
	m_emitter.publish<evnt::WaveUpdated>(0, WaveState::NOT_STARTED);
}

/* ----------------------------------- INPUT EVENTS --------------------------- */

void LevelState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);

	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case LevelInteractionState::FREE:
		case LevelInteractionState::INVALID:
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

		case LevelInteractionState::ROTATE:
			break;

		case LevelInteractionState::OPTIONS:
			// Click outside option menu closes it
			changeState(LevelInteractionState::FREE);
			break;

		case LevelInteractionState::BUILD:
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
		case LevelInteractionState::FREE:
			break;

		case LevelInteractionState::ROTATE:
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

		case LevelInteractionState::INVALID:
			break;

		case LevelInteractionState::OPTIONS:
			break;

		case LevelInteractionState::BUILD:
			break;

		default:
			break;
		}
	}
}

void LevelState::onRightClickDown(const evnt::RightClickDown& event) {
	if (m_game.emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case LevelInteractionState::FREE:
		case LevelInteractionState::INVALID:
		case LevelInteractionState::OPTIONS:
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
				} else if (m_game.registry.has<towerTag::SlowTower>(entityId)) {
					m_levelHud.setOptionsPosition(posWindow);
				} else {
					changeState(LevelInteractionState::INVALID);
				}
			}
			else {
				spdlog::warn("No valid entity on tile");
				changeState(LevelInteractionState::INVALID);
			}
			break;
		}

		case LevelInteractionState::ROTATE:
			break;

		case LevelInteractionState::BUILD:
			//Give the count back in the shop
			if (m_game.registry.has<entityTag::Mirror>(m_lastSelectedEntity)) {
				m_game.progression.setMirrorNumber(m_game.progression.getMirrorNumbers() + 1);
			}
			else if (m_game.registry.has<towerTag::SlowTower>(m_lastSelectedEntity)) {
				m_game.progression.setSlowNumber(m_game.progression.getSlowNumbers() + 1);
			}
			//Reset
			m_game.emitter.entityBeingPlaced = false;
			changeState(LevelInteractionState::FREE);
			m_game.registry.destroy(m_lastSelectedEntity);
			m_lastSelectedEntity = entt::null;
			//Update info bar
			m_game.emitter.publish<evnt::ProgressionUpdated>();
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
		case LevelInteractionState::FREE:
		{
			std::uint32_t entityId = m_game.level->getEntityOnTileFromProjCoord(event.mousePos.x, event.mousePos.y);
			if (m_game.registry.valid(entityId)) {
				if (m_game.registry.has<entityTag::Mirror>(entityId)) {
					m_emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
				} else if (m_game.registry.has<towerTag::LaserTower>(entityId)) {
					// TODO handle activated and desactivated towers
					m_emitter.publish<evnt::ChangeCursor>(CursorType::ACTIVATE);
				}
			} else {
				m_emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
			}
			break;
		}

		case LevelInteractionState::ROTATE:
			m_game.emitter.publish<evnt::SelectRotation>(event.mousePos);
			break;

		case LevelInteractionState::INVALID:
			break;

		case LevelInteractionState::OPTIONS:
			break;

		case LevelInteractionState::BUILD:
			// TODO follow mouse with transparent entity to be built
			break;
		default:
			break;
		}
	}
}
