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
#include "gui/level-hud/level-hud.hpp"

class Game; // Forward declaration

enum LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};

enum GameplayState {
	PLAY,
	WIN,
	LOOSE
};

class LevelState : public IGameState {
public:
	LevelState(Game& game);
	virtual ~LevelState();

	void enter() override;
	void update(float deltatime) override;
	void exit() override;

	GameplayState checkVictoryConditions();

	// Events
	void onLeftClickUp(const evnt::LeftClickUp& event) override;
	void onLeftClickDown(const evnt::LeftClickDown& event) override;
	void onRightClickDown(const evnt::RightClickDown& event) override;
	void onMouseMove(const evnt::MouseMove& event) override;

	// Getters
	LevelInteractionState getInteractionState();

	// Setters
	void changeState(LevelInteractionState state);

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	LevelHud m_levelHud;
	LevelInteractionState m_state;

	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;

	unsigned int m_invalidTimeCounter;
	unsigned int m_invalidTimeMax;
};
