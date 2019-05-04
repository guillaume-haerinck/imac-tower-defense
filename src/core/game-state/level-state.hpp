#pragma once

#include "i-game-state.hpp"
#include "core/game.hpp"

class LevelState {
public:
	LevelState();
	virtual ~LevelState();

	void update(Game& game, double deltatime);
};
