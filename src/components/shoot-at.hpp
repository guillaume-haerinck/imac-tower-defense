#pragma once

namespace cmpt {
	struct ShootAt {

		int loadingTime;
		int timer;

		ShootAt(int loadingTime) : loadingTime(loadingTime), timer(0) {}
	};
}