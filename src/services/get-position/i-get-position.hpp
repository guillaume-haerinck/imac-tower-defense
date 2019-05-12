#pragma once
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "events/handlers/event-emitter.hpp"

class IGetPosition {
public:
	virtual glm::vec2 get(unsigned int entityId) = 0;
	virtual void setRegistry(entt::DefaultRegistry* registry) = 0;
	virtual void setEmitter(EventEmitter* emitter) = 0;
};

