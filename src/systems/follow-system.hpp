#pragma once

#include "system.hpp"

class FollowSystem : public System {
public:
	FollowSystem(entt::DefaultRegistry& registry);
	void update(double deltatime);
};
