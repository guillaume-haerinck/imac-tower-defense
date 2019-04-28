#pragma once

namespace evnt {
	struct StartWave {
		StartWave(int nbEnemyToSpawn) : nbEnemyToSpawn(nbEnemyToSpawn) {}

		int nbEnemyToSpawn;
	};
}