#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <memory>

#include "i-game-state.hpp"
#include "entity-factories/mirror-factory.hpp"
#include "entity-factories/tower-factory.hpp"
#include "events/inputs/left-click-down.hpp"
#include "events/inputs/left-click-up.hpp"
#include "events/inputs/right-click-down.hpp"
#include "events/inputs/mouse-move.hpp"
#include "events/inputs/mouse-scrolled.hpp"
#include "gui/level-hud/level-hud.hpp"
#include "events/interactions/construct-selection.hpp"

class Game; // Forward declaration

enum class LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};

class LevelState : public IGameState {
public:
	LevelState(Game& game);
	virtual ~LevelState();

	void enter() override;
	void update(float deltatime) override;
	void exit() override;

	// Events
	void onLeftClickUp(const evnt::LeftClickUp& event) override;
	void onLeftClickDown(const evnt::LeftClickDown& event) override;
	void onRightClickDown(const evnt::RightClickDown& event) override;
	void onMouseMove(const evnt::MouseMove& event) override;
	void onMouseScrolled(const evnt::MouseScrolled& event) override;

	// Getters
	LevelInteractionState getInteractionState() const;

	// Setters
	void changeState(LevelInteractionState state);

private:
	void handleVictoryConditions();
	void handleConstructions();

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	LevelHud m_levelHud;
	LevelInteractionState m_state;

	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;

	unsigned int m_invalidTimeCounter;
	unsigned int m_invalidTimeMax;
	ConstructibleType m_constructType;
	std::uint32_t m_lastSelectedEntity;
	bool m_bWaveDone;
};


/* TODO listen to event here ? Or use a camera class, or even a camera service
	else if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
		viewTranslation.y++;
		viewMat = glm::translate(viewMat, glm::vec3(0.0f, 1.0f, 0.0f));
	} else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
		viewTranslation.y--;
		viewMat = glm::translate(viewMat, glm::vec3(0.0f, -1.0f, 0.0f));
	} else if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
		viewTranslation.x--;
		viewMat = glm::translate(viewMat, glm::vec3(-1.0f, 0.0f, 0.0f));
	} else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
		viewTranslation.x++;
		viewMat = glm::translate(viewMat, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	// TODO ameliorer translation quand proche du bord
	// FIXME
	viewScale += 0.1f;
	viewTranslation = glm::vec2(normMousePos.x * WIN_RATIO, normMousePos.y);
	viewMat = glm::translate(viewMat, glm::vec3(normMousePos.x * WIN_RATIO, normMousePos.y, 0.0f));
	viewMat = glm::scale(viewMat, glm::vec3(1.1f, 1.1f, 0.0f));
	viewMat = glm::translate(viewMat, glm::vec3(-normMousePos.x * WIN_RATIO, -normMousePos.y, 0.0f));
*/