#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>

#include "i-game-state.hpp"
#include "events/inputs/left-click-down.hpp"
#include "events/inputs/left-click-up.hpp"
#include "events/inputs/mouse-move.hpp"
#include "gui/level-exit/level-exit.hpp"

class Game; // Forward declaration

class LevelExitState : public IGameState {
public:
	LevelExitState(Game& game);
	virtual ~LevelExitState();

	void enter() override;
	void update(float deltatime) override;
	void exit() override;

	// Events
	void onLeftClickUp(const evnt::LeftClickUp& event) override;
	void onLeftClickDown(const evnt::LeftClickDown& event) override;
	void onMouseMove(const evnt::MouseMove& event) override;

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	LevelExit m_levelExit;
};
