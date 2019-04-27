#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "system.hpp"
#include "components/transform.hpp"

/* TODO use framebuffer */

class RenderSystem : public System {
public:
    RenderSystem(entt::DefaultRegistry& registry);
    void update(glm::mat4& view, glm::mat4& projection);

private:
    glm::mat4 getModelMatrix(cmpt::Transform& transform);
};
