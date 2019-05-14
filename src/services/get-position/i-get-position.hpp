#pragma once
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "events/handlers/event-emitter.hpp"

class IGetPosition {
public:
	virtual glm::vec4 getColour(unsigned int entityId) = 0;
	virtual glm::vec2 get(unsigned int entityId) = 0;
	virtual glm::vec4 blend(glm::vec4 col1, glm::vec4 col2) = 0;
	virtual float getVelocityMultiplier(unsigned int entityId) = 0;
	virtual void setRegistry(entt::DefaultRegistry* registry) = 0;
	virtual void setEmitter(EventEmitter* emitter) = 0;
};

