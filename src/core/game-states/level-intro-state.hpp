#pragma once

#include "i-game-state.hpp"

class Game; // Forward declaration

class LevelIntroState : public IGameState {
public:
	LevelIntroState(Game& game);
	virtual ~LevelIntroState();

	void enter() override;
	void update(float deltatime) override;
	void exit() override;
};
