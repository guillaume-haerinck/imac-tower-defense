#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "graphics/index-buffer.hpp"
#include "graphics/shader.hpp"
#include "components/primitive.hpp"

class PrimitiveFactory {
public:
    PrimitiveFactory();

    cmpt::Primitive createRect(glm::vec4 color, glm::vec2 displaySize);
    cmpt::Primitive createRectOutline(glm::vec4 color, glm::vec2 displaySize);

    // TODO circle and line

private:
    Shader m_shaderBasic;
};
