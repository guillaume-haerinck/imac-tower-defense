#pragma once

#include "system.hpp"

/*
    TODO pathfinding + State machine
	// Breakdown into multiple systems ?
*/

class AiSystem : public System {
public:
    AiSystem(entt::DefaultRegistry& registry);
    void update();
};
