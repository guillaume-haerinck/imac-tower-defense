#pragma once

#include <glad/glad.h>
#include "graphics/shader.hpp"
#include "graphics/index-buffer.hpp"

namespace cmpt {
struct Primitive {
    Primitive(glm::vec4 color, unsigned int vaID,  GLenum type, Shader* shader, unsigned int vertexCount) 
    : color(color), vaID(vaID), type(type), shader(shader), vertexCount(vertexCount) {}

    glm::vec4 color;
    unsigned int vaID;
    unsigned int vertexCount;
    GLenum type;
    Shader* shader;
};
}
