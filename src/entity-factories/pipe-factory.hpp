#pragma once

#include "factory.hpp"

class PipeFactory : public Factory {
public:
	PipeFactory(entt::DefaultRegistry& registry);
};

