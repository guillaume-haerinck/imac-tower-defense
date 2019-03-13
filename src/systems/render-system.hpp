#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "entt/entt.hpp"

#include "components/transform.hpp"

class RenderSystem {
public:
    RenderSystem();
    ~RenderSystem();

    void update(entt::registry<>& registry, glm::mat4& view, glm::mat4& projection);

private:
    glm::mat4 getModelMatrix(cmpt::Transform& transform);
};
