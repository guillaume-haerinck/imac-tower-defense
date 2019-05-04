#pragma once

#include "core/game.hpp"

enum GameState {
	WELCOME_SCREEN,
	LEVEL,
	CINEMATIC,
	GAME_OVER
};

/*
class IGameState {
public:
	virtual void update(Game& game, double deltatime) = 0;
};
*/
