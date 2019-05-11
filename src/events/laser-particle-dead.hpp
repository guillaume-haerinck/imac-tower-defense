#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct LaserParticleDead {
		LaserParticleDead(glm::vec2 position) : position(position) {}

		glm::vec2 position;
	};
}