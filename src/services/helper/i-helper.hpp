#pragma once
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "events/handlers/event-emitter.hpp"
#include "core/level/level.hpp"

class IHelper {
public:
	virtual glm::vec4 getColour(std::uint32_t entityId) = 0;
	virtual float getAlpha(std::uint32_t entityId) = 0;
	virtual glm::vec2 getPosition(std::uint32_t entityId) = 0;
	virtual glm::vec2 getPositionTowerTip(std::uint32_t entityId) = 0;
	virtual glm::vec2 getScale(std::uint32_t entityId) = 0;
	virtual glm::vec4 blend(glm::vec4 col1, glm::vec4 col2) = 0;
	virtual float getVelocityMultiplier(std::uint32_t entityId) = 0;
	virtual bool mouseIsOn(std::uint32_t entityId) = 0;
	virtual glm::vec2 getScreenShake() = 0;
	virtual void updateScreenShake(float deltatime) = 0;
	virtual void startScreenShake(float duration) = 0;

	virtual std::uint32_t getTile(unsigned int x, unsigned int y) = 0;
	virtual std::uint32_t getTileFromProjCoord(float x, float y) = 0;
	virtual std::uint32_t getEntityOnTileFromProjCoord(float x, float y) = 0;

	virtual void setRegistry(entt::DefaultRegistry* registry) = 0;
	virtual void setEmitter(EventEmitter* emitter) = 0;
	virtual void setLevel(Level* level) = 0;
};

