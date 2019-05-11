#pragma once

#include "i-game-state.hpp"

class Game; // Forward declaration

class LevelExitState : public IGameState {
public:
	LevelExitState(Game& game);
	virtual ~LevelExitState();

	void enter() override;
	void update(float deltatime) override;
	void exit() override;
};
