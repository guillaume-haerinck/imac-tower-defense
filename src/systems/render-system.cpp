#include "render-system.hpp"

#include "logger/gl-log-handler.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "components/primitive.hpp"
#include "core/tags.hpp"

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::update(entt::DefaultRegistry& registry, glm::mat4& view, glm::mat4& projection) {
    /* 
       We use multiple loops instead of a unique one with conditions that grabs missing components.
       Because it is faster, cache-friendly, and easier to debug as rendered entities are separated by types.
    */
    
    registry.view<renderTag::Atlas, cmpt::Transform, cmpt::Sprite, cmpt::SpriteAnimation>().each([&](auto entity, auto, cmpt::Transform& transform, cmpt::Sprite& sprite, cmpt::SpriteAnimation& animation) {
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
        GLCall(glBindTexture(sprite.target, sprite.textureID));
        sprite.ib->bind();

        glm::mat4 mvp = projection * view * getModelMatrix(transform);
        
        sprite.shader->setUniformMat4f("u_mvp", mvp);
        sprite.shader->setUniform1i("u_activeTile", animation.activeTile);
        GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));
    });

    registry.view<renderTag::Single, cmpt::Transform, cmpt::Sprite>().each([&](auto entity, auto, cmpt::Transform& transform, cmpt::Sprite& sprite) {
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
        GLCall(glBindTexture(sprite.target, sprite.textureID));
        sprite.ib->bind();

        glm::mat4 mvp = projection * view * getModelMatrix(transform);
        
        sprite.shader->setUniformMat4f("u_mvp", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));
    });


/*
    registry.view<renderTag::Single, cmpt::Transform, cmpt::Primitive>().each([&](auto entity, auto, cmpt::Transform& transform, cmpt::Primitive& primitive) {
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        sprite.ib->bind();

        glm::mat4 mvp = projection * view * getModelMatrix(transform);
        
        primitive.shader->setUniformMat4f("u_mvp", mvp);
        primitive.shader->setUniform4f("u_color", primitive.color.r, primitive.color.g, primitive.color.b, primitive.color.a); // TODO check if valid with vec4 uniform
        // GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr)); // No index buffer
    });
    */
}

glm::mat4 RenderSystem::getModelMatrix(cmpt::Transform& transform) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform.position);
    glm:: mat4 rotation = glm::toMat4(transform.rotation);
    model *= rotation;
    model = glm::scale(model, transform.scale);
    return model;
}
