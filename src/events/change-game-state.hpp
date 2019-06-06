#pragma once

#include "core/game-states/i-game-state.hpp"

namespace evnt {
	struct ChangeGameState {
		ChangeGameState(GameState state = GameState::TITLE_SCREEN, int subState = -1) : state(state), subState(subState) {}

		GameState state;
		int subState;
	};
}
