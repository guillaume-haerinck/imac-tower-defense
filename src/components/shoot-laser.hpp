#pragma once

#include "services/random/i-random.hpp"
#include "spdlog/spdlog.h"

namespace cmpt {
	struct ShootLaser {
		ShootLaser(bool isActiv=false) : isActiv(isActiv) {
			IRandom& random = entt::ServiceLocator<IRandom>::ref();
			col = glm::vec3(122 + random.random(-25, 25), 249 + random.random(-25, 25), 237 + random.random(-75, 75));
		}
		bool isActiv;
		glm::vec3 col;
	};
}
