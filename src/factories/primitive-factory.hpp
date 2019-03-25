#pragma once

#include <glad/glad.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "graphics/index-buffer.hpp"
#include "components/primitive.hpp"

class PrimitiveFactory {
public:
    PrimitiveFactory(entt::DefaultRegistry& registry);
    ~PrimitiveFactory();

    cmpt::Primitive createRect(glm::vec4 color, glm::vec2 displaySize, GLenum usage);

private:
    Shader m_shaderBasic;
    entt::DefaultRegistry& m_registry;
};
