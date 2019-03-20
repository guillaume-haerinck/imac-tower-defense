#include "render-system.hpp"

#include "logger/gl-log-handler.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::update(entt::DefaultRegistry& registry, glm::mat4& view, glm::mat4& projection) {
    registry.view<cmpt::Transform, cmpt::Sprite, cmpt::SpriteAnimation>().each([this, view = view, projection = projection](const uint32_t entity, cmpt::Transform& transform, cmpt::Sprite& sprite, cmpt::SpriteAnimation& animation) {
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images
        GLCall(glBindTexture(sprite.target, sprite.textureID));

        glm::mat4 mvp = projection * view * getModelMatrix(transform);
        sprite.ib->bind();
        sprite.shader->setUniformMat4f("u_mvp", mvp);
        sprite.shader->setUniform1i("u_activeTile", animation.activeTile);
        GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));
    });
}

glm::mat4 RenderSystem::getModelMatrix(cmpt::Transform& transform) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform.position);
    glm:: mat4 rotation = glm::toMat4(transform.rotation);
    model *= rotation;
    model = glm::scale(model, transform.scale);
    return model;
}
