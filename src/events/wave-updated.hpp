#pragma once

#include "systems/wave-system.hpp"

namespace evnt {
	struct WaveUpdated {
		WaveUpdated(int timer, WaveState state) : timer(timer), state(state) {}

		int timer;
		WaveState state;
	};
}
