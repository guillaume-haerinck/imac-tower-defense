#pragma once

namespace cmpt {
	struct Trajectory { Trajectory(std::vector<glm::vec2> traj) : traj(traj), currentTarget(1) {}
		std::vector<glm::vec2> traj;
		int currentTarget;
	};
}
