#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "components/transform.hpp"

/* TODO use framebuffer */

class RenderSystem : public ISystem {
public:
    RenderSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, glm::mat4& viewMat, glm::mat4& projMat);
    void update(float deltatime) override;

private:
    glm::mat4 getModelMatrix(cmpt::Transform& transform) const;
	const glm::mat4& m_view;
	const glm::mat4& m_projection;
};
