#pragma once

#include "factory.hpp"

class ProjectileFactory : public Factory {
public:
	ProjectileFactory(entt::DefaultRegistry& registry);
};

