#pragma once

namespace evnt {
	struct StartWave {
		StartWave(int nbEnemyToSpawn, int spawnRate) : nbEnemyToSpawn(nbEnemyToSpawn), spawnRate(spawnRate) {}

		int nbEnemyToSpawn;
		int spawnRate;
	};
}