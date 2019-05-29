#pragma once

#include "i-helper.hpp"
#include "core/level/level.hpp"

class Game; // Forward declaration

class HelperService : public IHelper {
public:
	HelperService();

	glm::vec4 getColour(std::uint32_t entityId) override;
	float getAlpha(std::uint32_t entityId) override;
	glm::vec4 blend(glm::vec4 col1, glm::vec4 col2) override;
	glm::vec2 getPosition(std::uint32_t entityId) override;
	glm::vec2 getScale(std::uint32_t entityId) override;
	float getVelocityMultiplier(std::uint32_t entityId) override;

	bool mouseIsOn(std::uint32_t entityId) override;

	void setRegistry(entt::DefaultRegistry* registry) override;
	void setEmitter(EventEmitter* emitter) override;
	void setLevel(Level* level) override;

private:
	entt::DefaultRegistry* m_registry;
	EventEmitter* m_emitter;
	Level* m_level;
};