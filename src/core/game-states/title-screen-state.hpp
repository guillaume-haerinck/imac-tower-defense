#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <memory>

#include "i-game-state.hpp"
#include "events/inputs/left-click-down.hpp"
#include "events/inputs/left-click-up.hpp"
#include "events/inputs/mouse-move.hpp"
#include "gui/title-screen/title-screen.hpp"

class Game; // Forward declaration

class TitleScreenState : public IGameState {
public:
	TitleScreenState(Game& game);
	virtual ~TitleScreenState();

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
	TitleScreen m_titleScreen;
};
