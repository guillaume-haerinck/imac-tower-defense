#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "graphics/index-buffer.hpp"
#include "components/sprite.hpp"

class SpriteFactory {
public:
    SpriteFactory();

    cmpt::Sprite createSingle(const std::string& textureFilepath, glm::vec2 displaySize);
    cmpt::Sprite createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize);
    // TODO batch rendering with glTextureView ? https://learnopengl.com/Advanced-OpenGL/Instancing for all since they share vertex data

private:
	// TODO make shader static because shared among each instance
	// -> Init problem because glad is not up at compile time to create the shader

    Shader m_shaderTex;
    Shader m_shaderTexArray;
    IndexBuffer m_ib;    // All sprites shares the same index buffer
};
