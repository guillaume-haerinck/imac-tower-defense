#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "graphics/shader.hpp"
#include "graphics/index-buffer.hpp"

namespace cmpt {
struct Primitive {
    Primitive(glm::vec4 color = glm::vec4(0), unsigned int vaID = 0,  GLenum type = 0, Shader* shader = nullptr, unsigned int vertexCount = 0) 
    : color(color), vaID(vaID), type(type), shader(shader), vertexCount(vertexCount) {}

    glm::vec4 color;
    unsigned int vaID;
    unsigned int vertexCount;
    GLenum type;
    Shader* shader;
};
}
