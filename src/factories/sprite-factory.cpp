#include "sprite-factory.hpp"

#include "debug/gl-error-handling.hpp"
#include "render/vertex-array.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include "render/texture-array.hpp"

SpriteFactory::SpriteFactory(entt::registry<>& registry)
: m_shaderBasic("ressources/shaders/basic-mvp.vert", "ressources/shaders/basic-texture.frag"),
  m_shaderTexArray("ressources/shaders/basic-mvp.vert", "ressources/shaders/basic-texture-array2d.frag"),
  m_registry(registry)
{
    /* Index buffer */
	unsigned int indices[] = {
		0, 1, 2, // First triangle
		3, 1, 2  // Second triangle
	};
	unsigned int arraySize = sizeof(indices) / sizeof(unsigned int);
    m_ib.init(indices, arraySize);
    m_ib.unbind();
}

SpriteFactory::~SpriteFactory() {
    m_registry.view<cmpt::Sprite>().each([&](auto entity, cmpt::Sprite& sprite) {
        GLCall(glDeleteTextures(1, &sprite.textureID));
        GLCall(glDeleteVertexArrays(1, &sprite.vaID));
    });
}

cmpt::Sprite SpriteFactory::create(const std::string& textureFilepath, glm::vec2 displaySize, GLenum usage) {
	/* Vertex buffer */
    // TODO add enum to specify the pivot point ? By default center
	float positions[] = {
        // Pos                           // UV
		-displaySize.x,  displaySize.y,  0.0f, 1.0f, // 0 topleft
		 displaySize.x,  displaySize.y,  1.0f, 1.0f, // 1 topright
		-displaySize.x, -displaySize.y,  0.0f, 0.0f, // 2 bottomleft
		 displaySize.x, -displaySize.y,  1.0f, 0.0f  // 3 bottomright
	};
    unsigned int arraySize = sizeof(positions) / sizeof(float);
    VertexBuffer vb(positions, arraySize * sizeof(float), usage);
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)
    vbLayout.push<float>(2); // Texture coordinates

    /* Vertex array */
    VertexArray va;
	va.addBuffer(vb, vbLayout);

    /* Uniforms */
    m_shaderBasic.bind();
    m_shaderBasic.setUniformMat4f("u_mvp", glm::mat4(1.0f));
    Texture texture(textureFilepath);
    texture.bind();
	m_shaderBasic.setUniform1i("u_texture", 0);
    
    /* Cleanup */
    m_shaderBasic.unbind();
    va.unbind();
    vb.unbind();
    texture.unbind();
    
    /* Send IDs */
    cmpt::Sprite mySprite;
    mySprite.shader = &m_shaderBasic;
    mySprite.ib = &m_ib;
    mySprite.textureID = texture.getID();
    mySprite.vaID = va.getID();
    return mySprite;
}

cmpt::Sprite SpriteFactory::createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, GLenum usage, glm::vec2 tileSize) {
    float positions[] = {
        // Pos                           // Inverted UV to start at topleft
		-displaySize.x,  displaySize.y,  0.0f, 0.0f, // 0 topleft
		 displaySize.x,  displaySize.y,  1.0f, 0.0f, // 1 topright
		-displaySize.x, -displaySize.y,  0.0f, 1.0f, // 2 bottomleft
		 displaySize.x, -displaySize.y,  1.0f, 1.0f  // 3 bottomright
	};
    unsigned int arraySize = sizeof(positions) / sizeof(float);
    VertexBuffer vb(positions, arraySize * sizeof(float), usage);
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)
    vbLayout.push<float>(2); // Texture coordinates

    /* Vertex array */
    VertexArray va;
	va.addBuffer(vb, vbLayout);

    /* Uniforms */
    m_shaderTexArray.bind();
    m_shaderTexArray.setUniformMat4f("u_mvp", glm::mat4(1.0f));
    TextureArray texture(textureFilepath, (GLsizei) tileSize.x, (GLsizei) tileSize.y);
    texture.bind();
	m_shaderTexArray.setUniform1i("u_texture", 0);
	m_shaderTexArray.setUniform1i("u_activeTile", 0);
    
    /* Cleanup */
    m_shaderTexArray.unbind();
    va.unbind();
    vb.unbind();
    texture.unbind();
    
    /* Send IDs */
    cmpt::Sprite mySprite;
    mySprite.shader = &m_shaderTexArray;
    mySprite.ib = &m_ib;
    mySprite.textureID = texture.getID();
    mySprite.vaID = va.getID();
    return mySprite;
}