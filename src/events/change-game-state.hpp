#pragma once

#include "core/game-states/i-game-state.hpp"

namespace evnt {
	struct ChangeGameState {
		ChangeGameState(GameState state, int subState) : state(state), subState(subState) {}

		GameState state;
		int subState;
	};
}
