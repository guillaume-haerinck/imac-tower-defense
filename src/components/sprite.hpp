#pragma once

#include <glad/glad.h>
#include "graphics/shader.hpp"
#include "graphics/index-buffer.hpp"

namespace cmpt {
struct Sprite {
    unsigned int textureID;
    unsigned int vaID;
    GLenum target;
    Shader* shader;
    IndexBuffer* ib;
};
}
