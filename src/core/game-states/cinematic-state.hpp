#pragma once

#include "i-game-state.hpp"

class Game; // Forward declaration

class CinematicState : public IGameState {
public:
	CinematicState(Game& game);
	virtual ~CinematicState();

	void enter() override;
	void update(float deltatime) override;
	void exit() override;
};
