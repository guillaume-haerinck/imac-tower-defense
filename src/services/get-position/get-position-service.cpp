#pragma once

#include "get-position-service.hpp"

#include "components/transform.hpp"
#include "components/attached-to.hpp"
#include "components/wiggle.hpp"
#include "components/move-towards-mouse.hpp"
#include "components/shake.hpp"
#include "components/tint-colour.hpp"
#include "components/velocity.hpp"
#include "core/constants.hpp"

#include <SDL2/SDL.h>
#include "core/maths.hpp"

#include "spdlog/spdlog.h"

GetPositionService::GetPositionService() {}

glm::vec4 GetPositionService::getColour(unsigned int entityId) {
	glm::vec4 actualColour = glm::vec4(0.0f);
	if (m_registry->has<cmpt::TintColour>(entityId)) {
		cmpt::TintColour tint = m_registry->get<cmpt::TintColour>(entityId);
		actualColour = blend(actualColour, tint.col);
		if (tint.bOneTimeOnly) {
			m_registry->remove<cmpt::TintColour>(entityId);
		}
	}
	if (m_registry->has<cmpt::Velocity>(entityId)) {
		actualColour = blend(actualColour, glm::vec4( glm::vec3(0.49,0.62,0.84) , 1-getVelocityMultiplier(entityId)) );
	}
	return actualColour;
}

float GetPositionService::getVelocityMultiplier(unsigned int entityId) {
	cmpt::Velocity velocity = m_registry->get<cmpt::Velocity>(entityId);
	return imaths::rangeMapping(velocity.multiplierAge, 0, velocity.multiplierLifespan, velocity.velMultiplier, 1);
}

glm::vec4 GetPositionService::blend(glm::vec4 col1, glm::vec4 col2) {
	float aSum = col1.a + col2.a;
	if (aSum == 0) {
		return glm::vec4(0.0f);
	}
	float alpha1 = col1.a / aSum;
	float alpha2 = col2.a / aSum;
	glm::vec4 col = alpha1 * col1 + alpha2 * col2;
	col.a = imaths::max(col1.a , col2.a );
	return col;
}

glm::vec2 GetPositionService::get(unsigned int entityId) {
	glm::vec2 actualPos = glm::vec2(0.0f);
	//Transform
	if (m_registry->has<cmpt::Transform>(entityId)) {
		actualPos += m_registry->get<cmpt::Transform>(entityId).position;
	}
	//Wiggle
	if (m_registry->has<cmpt::Wiggle>(entityId)) {
		cmpt::Wiggle& wiggle = m_registry->get<cmpt::Wiggle>(entityId);
		IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
		float r = wiggle.amplitude*(1 + randomService.noise((float)SDL_GetTicks() *0.0003 + wiggle.noiseOffset)) / 2;
		float agl = 5 * imaths::TAU*randomService.noise((float)SDL_GetTicks() *0.000025 + wiggle.noiseOffset + 50);
		glm::vec2 dl = r * glm::vec2(cos(agl), sin(agl));
		actualPos += dl;
	}
	//Shake
	if (m_registry->has<cmpt::Shake>(entityId)) {
		actualPos += m_registry->get<cmpt::Shake>(entityId).offset;
	}
	//Attached to another entity
	if (m_registry->has<cmpt::AttachedTo>(entityId)) {
		unsigned int mainEntityId = m_registry->get<cmpt::AttachedTo>(entityId).entityId;
		if (m_registry->valid(mainEntityId)) {
			glm::vec2 mainPos = get(mainEntityId);
			actualPos += mainPos;
			//Move towards mouse
			//Must be added last because takes into account all the other accumulated positions as a center for its movement
			if (m_registry->has<cmpt::MoveTowardsMouse>(entityId)) {
				cmpt::MoveTowardsMouse& move = m_registry->get<cmpt::MoveTowardsMouse>(entityId);
				float agl = atan2(m_emitter->mousePos.y - actualPos.y, m_emitter->mousePos.x * WIN_RATIO - actualPos.x);
				actualPos += move.maxDist * glm::vec2(cos(agl), sin(agl));
			}
		}
		else {
			m_registry->destroy(entityId);
		}
	}
	return actualPos;
}

void GetPositionService::setRegistry(entt::DefaultRegistry* registry) {
	m_registry = registry;
}
void GetPositionService::setEmitter(EventEmitter* emitter) {
	m_emitter = emitter;
}