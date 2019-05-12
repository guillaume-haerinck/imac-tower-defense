#pragma once

#include "i-get-position.hpp"

class GetPositionService : public IGetPosition {
public:
	GetPositionService();

	glm::vec2 get(unsigned int entityId) override;

	void setRegistry(entt::DefaultRegistry* registry) override;
	void setEmitter(EventEmitter* emitter) override;

private:
	entt::DefaultRegistry* m_registry;
	EventEmitter* m_emitter;
};