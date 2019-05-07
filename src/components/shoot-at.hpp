#pragma once

namespace cmpt {
	struct ShootAt {

		int loadingTime;
		int timer;
		float range;

		ShootAt(int loadingTime) : loadingTime(loadingTime), timer(loadingTime-1) {}
	};
}