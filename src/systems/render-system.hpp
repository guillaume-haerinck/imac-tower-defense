#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "system.hpp"
#include "components/transform.hpp"

/* TODO use framebuffer */

class RenderSystem : public System {
public:
    RenderSystem(entt::DefaultRegistry& registry, glm::mat4& viewMat, glm::mat4& projMat);
    void update();

private:
    glm::mat4 getModelMatrix(cmpt::Transform& transform);
	glm::mat4& m_view;
	glm::mat4& m_projection;
};
