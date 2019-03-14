#pragma once

#include <glad/glad.h>
#include "entt/entt.hpp"
#include "glm/glm.hpp"

#include "graphics/index-buffer.hpp"
#include "components/sprite.hpp"

class SpriteFactory {
public:
    SpriteFactory(entt::registry<>& registry);
    ~SpriteFactory();

    cmpt::Sprite create(const std::string& textureFilepath, glm::vec2 displaySize, GLenum usage);
    cmpt::Sprite createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, GLenum usage, glm::vec2 tileSize);
    // TODO batch rendering with glTextureView ?

private:
    Shader m_shaderBasic;
    Shader m_shaderTexArray;
    IndexBuffer m_ib;    // All sprites shares the same index buffer
    float m_vertex;      // All sprites shares the same vertex data
    entt::registry<>& m_registry;
};
