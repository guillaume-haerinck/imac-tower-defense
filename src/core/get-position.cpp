#include "get-position.hpp"

#include "components/transform.hpp"
#include "components/attached-to.hpp"
#include "components/wiggle.hpp"
#include "components/move-towards-mouse.hpp"

#include <SDL2/SDL.h>
#include "core/maths.hpp"

glm::vec2 getPosition(entt::DefaultRegistry& registry, unsigned int entityId) {
	glm::vec2 actualPos = glm::vec2(0.0f);
	if (registry.has<cmpt::Transform>(entityId)) {
		actualPos += registry.get<cmpt::Transform>(entityId).position;
	}
	if (registry.has<cmpt::Wiggle>(entityId)) {
		cmpt::Wiggle& wiggle = registry.get<cmpt::Wiggle>(entityId);
		IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
		float r = wiggle.amplitude*(1 + randomService.noise((float)SDL_GetTicks() *0.0003 + wiggle.noiseOffset)) / 2;
		float agl = 5 * imaths::TAU*randomService.noise((float)SDL_GetTicks() *0.000025 + wiggle.noiseOffset + 50);
		glm::vec2 dl = r * glm::vec2(cos(agl), sin(agl));
		actualPos += dl;
	}
	//Must be added last because takes into account all the other accumulated positions as a center for its movement
	if (registry.has<cmpt::AttachedTo>(entityId)) {
		unsigned int mainEntityId = registry.get<cmpt::AttachedTo>(entityId).entityId;
		if (registry.valid(mainEntityId)) {
			glm::vec2 mainPos = registry.get<cmpt::Transform>(mainEntityId).position;
			actualPos += mainPos;
			if (registry.has<cmpt::MoveTowardsMouse>(entityId)) {
				cmpt::MoveTowardsMouse& move = registry.get<cmpt::MoveTowardsMouse>(entityId);
				//float agl = atan2(m_prevMousePos.y - actualPos.y, m_prevMousePos.x * WIN_RATIO - actualPos.x);
				float agl = 0;
				actualPos += move.maxDist * glm::vec2(cos(agl), sin(agl));
			}
		}
		else {
			registry.destroy(entityId);
		}
	}
	return actualPos;
}