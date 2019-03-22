#pragma once

#include <glad/glad.h>
#include "graphics/shader.hpp"
#include "graphics/index-buffer.hpp"

namespace cmpt {
struct Primitive {
    Primitive(glm::vec4 color, unsigned int vaID,  GLenum type, Shader* shader) 
    : color(color), vaID(vaID), type(type), shader(shader) {}

    glm::vec4 color;
    unsigned int vaID;
    GLenum type;
    Shader* shader;
};
}
