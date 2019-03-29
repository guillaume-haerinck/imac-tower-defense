#pragma once

#include <glad/glad.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "graphics/index-buffer.hpp"
#include "components/sprite.hpp"

class SpriteFactory {
public:
    SpriteFactory(entt::DefaultRegistry& registry);
    ~SpriteFactory();

    cmpt::Sprite create(const std::string& textureFilepath, glm::vec2 displaySize);
    cmpt::Sprite createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize);
    // TODO batch rendering with glTextureView ?

private:
    Shader m_shaderTex;
    Shader m_shaderTexArray;
    IndexBuffer m_ib;    // All sprites shares the same index buffer
    float m_vertex;      // All sprites shares the same vertex data
    entt::DefaultRegistry& m_registry;
};
