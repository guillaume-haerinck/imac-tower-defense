#pragma once

#include "get-position-service.hpp"

#include "components/transform.hpp"
#include "components/attached-to.hpp"
#include "components/wiggle.hpp"
#include "components/move-towards-mouse.hpp"
#include "components/shake.hpp"
#include "core/constants.hpp"

#include <SDL2/SDL.h>
#include "core/maths.hpp"

GetPositionService::GetPositionService() {}

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