#pragma once

namespace evnt {
	struct StartWave {
		StartWave(int spawnRate) : spawnRate(spawnRate) {}

		int spawnRate;
	};
}