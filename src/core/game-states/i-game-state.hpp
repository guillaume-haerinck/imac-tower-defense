#pragma once

#include "events/handlers/input-handler.hpp"

class Game; // Forward declaration

enum GameState {
	TITLE_SCREEN,
	LEVEL,
	CINEMATIC,
	GAME_OVER
};

class IGameState : public InputHandler {
public:
	IGameState(Game& game);

	virtual void enter() = 0;
	virtual void update(float deltatime) = 0;
	virtual void exit() = 0;

protected:
	Game& m_game;
};
