#pragma once

#include "services/random/i-random.hpp"
#include "spdlog/spdlog.h"

namespace cmpt {
	struct ShootLaser {
		ShootLaser(bool isActiv=false) : isActiv(isActiv) {
			IRandom& random = entt::ServiceLocator<IRandom>::ref();
			col = glm::vec3(122, 249, 237);
		}
		bool isActiv;
		glm::vec3 col;
	};
}
