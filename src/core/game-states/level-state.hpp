#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <memory>

#include "i-game-state.hpp"
#include "events/left-click-down.hpp"
#include "events/left-click-up.hpp"
#include "events/mouse-move.hpp"
#include "gui/level-hud.hpp"

class Game; // Forward declaration

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
	void onMouseMove(const evnt::MouseMove& event) override;

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	LevelHud m_levelHud;
};
