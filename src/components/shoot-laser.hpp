#pragma once

namespace cmpt {
	struct ShootLaser {
		ShootLaser(bool isActiv=false) : isActiv(isActiv) {}
		bool isActiv;
	};
}
