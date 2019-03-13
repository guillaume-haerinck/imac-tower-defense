#pragma once

#include "render/shader.hpp"
#include "render/index-buffer.hpp"

namespace cmpt {
struct Sprite {
    unsigned int textureID;
    unsigned int vaID;
    Shader* shader;
    IndexBuffer* ib;
};
}
