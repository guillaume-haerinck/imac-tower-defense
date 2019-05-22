#pragma once

#include "helper-service.hpp"

#include "components/transform.hpp"
#include "components/attached-to.hpp"
#include "components/wiggle.hpp"
#include "components/move-towards-mouse.hpp"
#include "components/shake.hpp"
#include "components/tint-colour.hpp"
#include "components/velocity.hpp"
#include "components/animated.hpp"
#include "components/animation-scale.hpp"
#include "components/animation-dark.hpp"
#include "components/hitbox.hpp"
#include "core/constants.hpp"
#include "core/tags.hpp"

#include <SDL2/SDL.h>
#include "core/maths.hpp"

#include "spdlog/spdlog.h"

HelperService::HelperService() {}

glm::vec4 HelperService::getColour(unsigned int entityId) {
	glm::vec4 actualColour = glm::vec4(0.0f);
	if (m_registry->has<cmpt::TintColour>(entityId)) {
		cmpt::TintColour tint = m_registry->get<cmpt::TintColour>(entityId);
		actualColour = blend(actualColour, tint.col);
		if (tint.bOneTimeOnly) {
			m_registry->remove<cmpt::TintColour>(entityId);
		}
	}
	//Blue when slowed
	if (m_registry->has<cmpt::Velocity>(entityId)) {
		actualColour = blend(actualColour, glm::vec4( glm::vec3(0.49,0.62,0.84) , 1-getVelocityMultiplier(entityId)) );
	}
	if (m_registry->has<cmpt::Animated>(entityId)) {
		cmpt::Animated& animated = m_registry->get<cmpt::Animated>(entityId);
		//Animation darken
		if (m_registry->has<cmpt::AnimationDark>(entityId)) {
			if (m_registry->get<cmpt::AnimationDark>(entityId).bForward) {
				actualColour = blend(actualColour , glm::vec4(0.0f,0.0f,0.0f, 1 - animated.age / animated.duration));
			}
			else {
				actualColour = blend(actualColour, glm::vec4(0.0f, 0.0f, 0.0f, animated.age / animated.duration));
			}
		}
	}
	//Highlight hovered tile
	if (m_registry->has<entityTag::Tile>(entityId)) {
		glm::vec2 tilePos = m_registry->get<cmpt::Transform>(entityId).position;
		if (m_emitter->entityBeingPlaced) {
			if (abs(m_emitter->mousePos.x*WIN_RATIO - tilePos.x) < TILE_SIZE / 2 && abs(m_emitter->mousePos.y - tilePos.y) < TILE_SIZE / 2) {
				if (m_registry->has<tileTag::Constructible>(entityId)) {
					actualColour = blend(actualColour, glm::vec4(0, 1, 0, 0.3));
				}
				else {
					actualColour = blend(actualColour, glm::vec4(1, 0, 0, 0.3));
				}
			}
		}
	}
	//Attached to another entity
	if (m_registry->has<cmpt::AttachedTo>(entityId)) {
		unsigned int mainEntityId = m_registry->get<cmpt::AttachedTo>(entityId).mainEntity;
		if (m_registry->valid(mainEntityId)) {
			actualColour = blend( actualColour, getColour(mainEntityId));
		}
		else {
			m_registry->destroy(entityId);
		}
	}
	return actualColour;
}

float HelperService::getVelocityMultiplier(unsigned int entityId) {
	cmpt::Velocity velocity = m_registry->get<cmpt::Velocity>(entityId);
	return imaths::rangeMapping(velocity.multiplierAge, 0, velocity.multiplierLifespan, velocity.velMultiplier, 1);
}

glm::vec4 HelperService::blend(glm::vec4 col1, glm::vec4 col2) {
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

glm::vec2 HelperService::getPosition(unsigned int entityId) {
	glm::vec2 actualPos = glm::vec2(0.0f);
	//Transform
	if (m_registry->has<cmpt::Transform>(entityId)) {
		actualPos += m_registry->get<cmpt::Transform>(entityId).position;
	}
	//Is on mouse
	if (m_registry->has<positionTag::IsOnHoveredTile>(entityId)) {
		unsigned int tileId = m_level->getTileFromProjCoord(m_emitter->mousePos.x, m_emitter->mousePos.y);
		if (m_registry->valid(tileId)) {
			actualPos += m_registry->get<cmpt::Transform>(tileId).position;
		}
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
		unsigned int mainEntityId = m_registry->get<cmpt::AttachedTo>(entityId).mainEntity;
		if (m_registry->valid(mainEntityId)) {
			actualPos += getPosition(mainEntityId);
		}
	}
	//Move towards mouse
	//Must be added last because takes into account all the other accumulated positions as a center for its movement
	if (m_registry->has<cmpt::MoveTowardsMouse>(entityId)) {
		float maxDist = m_registry->get<cmpt::MoveTowardsMouse>(entityId).maxDist;
		float agl = atan2(m_emitter->mousePos.y - actualPos.y, m_emitter->mousePos.x * WIN_RATIO - actualPos.x);
		actualPos += maxDist * glm::vec2(cos(agl), sin(agl));
	}
	return actualPos;
}

glm::vec2 HelperService::getScale(unsigned int entityId) {
	glm::vec2 actualScale = glm::vec2(1.0f);
	if (m_registry->valid(entityId)) {
		//Transform
		if (m_registry->has<cmpt::Transform>(entityId)) {
			actualScale *= m_registry->get<cmpt::Transform>(entityId).scale;
		}
		if (m_registry->has<cmpt::Animated>(entityId)) {
			cmpt::Animated& animated = m_registry->get<cmpt::Animated>(entityId);
			//Animation scale
			if (m_registry->has<cmpt::AnimationScale>(entityId)) {
				if (m_registry->get<cmpt::AnimationScale>(entityId).bForward) {
					actualScale *= animated.age / animated.duration;
				}
				else {
					actualScale *= 1 - animated.age / animated.duration;
				}
			}
		}
		//Attached to a main entity
		if (m_registry->has<cmpt::AttachedTo>(entityId)) {
			unsigned int mainId = m_registry->get<cmpt::AttachedTo>(entityId).mainEntity;
			if (m_registry->valid(mainId)) {
				actualScale *= getScale(mainId);
			}
			else {
				m_registry->destroy(entityId);
			}
		}
	}
	return actualScale;
}

bool HelperService::mouseIsOn(unsigned int entityId) {
	if (m_registry->has<cmpt::Hitbox>(entityId)) {
		cmpt::Hitbox& hitbox = m_registry->get<cmpt::Hitbox>(entityId);
		glm::vec2 pos = m_registry->get<cmpt::Transform>(entityId).position;
		return hitbox.radiusSq > (pos.x - m_emitter->mousePos.x*WIN_RATIO)*(pos.x - m_emitter->mousePos.x*WIN_RATIO) + (pos.y - m_emitter->mousePos.y)*(pos.y - m_emitter->mousePos.y);
	}
	else {
		return false;
	}
}

void HelperService::setRegistry(entt::DefaultRegistry* registry) {
	m_registry = registry;
}
void HelperService::setEmitter(EventEmitter* emitter) {
	m_emitter = emitter;
}
void HelperService::setLevel(Level* level) {
	m_level = level;
}