#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <array>
#include <SDL2/SDL.h>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "components/transform.hpp"
#include "components/sprite.hpp"
#include "events/interactions/change-cursor.hpp"

/* TODO use framebuffer */

class RenderSystem : public ISystem {
public:
    RenderSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, glm::mat4& viewMat, glm::mat4& projMat);
	virtual ~RenderSystem();

    void update(float deltatime) override;

private:
	void renderSprite(std::uint32_t entity, cmpt::Transform & transform, cmpt::Sprite & sprite) const;
    glm::mat4 getModelMatrix(unsigned int entityId) const;
	glm::mat4 getModelMatrix(cmpt::Transform& transform) const;
	const glm::mat4& m_view;
	const glm::mat4& m_projection;
	std::array<SDL_Cursor*, 5> cursors;
};
