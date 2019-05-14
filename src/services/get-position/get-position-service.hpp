#pragma once

#include "i-get-position.hpp"

class GetPositionService : public IGetPosition {
public:
	GetPositionService();

	glm::vec4 getColour(unsigned int entityId) override;
	glm::vec4 blend(glm::vec4 col1, glm::vec4 col2) override;
	glm::vec2 get(unsigned int entityId) override;
	float getVelocityMultiplier(unsigned int entityId) override;

	void setRegistry(entt::DefaultRegistry* registry) override;
	void setEmitter(EventEmitter* emitter) override;

private:
	entt::DefaultRegistry* m_registry;
	EventEmitter* m_emitter;
};