#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <memory>

#include "systems/render-system.hpp"
#include "events/left-click-down.hpp"
#include "events/left-click-up.hpp"
#include "i-game-state.hpp"
#include "gui/game-over.hpp"

class Game; // Forward declaration

class GameOverState : public IGameState {
public:
	GameOverState(Game& game);
	virtual ~GameOverState();

	void onEnter() override;
	void update(float deltatime) override;
	void onExit() override;

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	GameOver m_gameOver;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>> m_clickDownCon;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_clickUpCon;
};
