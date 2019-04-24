#include "render-system.hpp"

#include "logger/gl-log-handler.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "components/primitive.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::update(entt::DefaultRegistry& registry, glm::mat4& view, glm::mat4& projection) {
    /* 
        TODO find a way to use only a few glDraw by sharing buffer or using vertex array. Each draw call should draw all sprites of a particular type. For uniforms, transfer them to vertex attributes
        https://community.khronos.org/t/best-practices-to-render-multiple-2d-sprite-with-vbo/74096
    */

    registry.view<renderTag::Atlas, cmpt::Transform, cmpt::Sprite, cmpt::SpriteAnimation>().each([this, projection, view](auto entity, auto, cmpt::Transform& transform, cmpt::Sprite& sprite, cmpt::SpriteAnimation& animation) {
        // Binding
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
        GLCall(glBindTexture(sprite.target, sprite.textureID));
        sprite.ib->bind();

        // Updates
        glm::mat4 mvp = projection * view * this->getModelMatrix(transform);
        sprite.shader->setUniformMat4f("u_mvp", mvp);
        sprite.shader->setUniform1i("u_activeTile", animation.activeTile);
        GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

        // Unbinding
        sprite.ib->unbind();
        GLCall(glBindTexture(sprite.target, 0));
        GLCall(glBindVertexArray(0));
        sprite.shader->unbind();
    });

    registry.view<renderTag::Single, cmpt::Transform, cmpt::Sprite>().each([this, projection, view](auto entity, auto, cmpt::Transform& transform, cmpt::Sprite& sprite) {
        // Binding
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
        GLCall(glBindTexture(sprite.target, sprite.textureID));
        sprite.ib->bind();

        // Updates
        glm::mat4 mvp = projection * view * this->getModelMatrix(transform);
        sprite.shader->setUniformMat4f("u_mvp", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

        // Unbinding
        sprite.ib->unbind();
        GLCall(glBindTexture(sprite.target, 0));
        GLCall(glBindVertexArray(0));
        sprite.shader->unbind();
    });

    registry.view<cmpt::Transform, cmpt::Primitive>().each([this, projection, view](auto entity, cmpt::Transform& transform, cmpt::Primitive& primitive) {
        // Binding
        primitive.shader->bind();
        GLCall(glBindVertexArray(primitive.vaID));

        // Updates
        glm::mat4 mvp = projection * view * this->getModelMatrix(transform);
        primitive.shader->setUniformMat4f("u_mvp", mvp);
        primitive.shader->setUniform4f("u_color", primitive.color.r, primitive.color.g, primitive.color.b, primitive.color.a);
        GLCall(glDrawArrays(primitive.type, 0, primitive.vertexCount));

        // Unbinding
        GLCall(glBindVertexArray(0));
        primitive.shader->unbind();
    });
}

glm::mat4 RenderSystem::getModelMatrix(cmpt::Transform& transform) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(transform.position, transform.zIndex));
	// TODO rotation
    //glm:: mat4 rotation = glm::toMat4(transform.axis);
    //model *= rotation;
    model = glm::scale(model, glm::vec3(transform.scale, 0.0f));
    return model;
}
