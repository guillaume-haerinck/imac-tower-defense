#include "render-system.hpp"

#include "logger/gl-log-handler.hpp"
#include "components/sprite.hpp"

RenderSystem::RenderSystem() {}

RenderSystem::~RenderSystem() {}

void RenderSystem::update(entityx::EntityX& registry, glm::mat4& view, glm::mat4& projection) {
    registry.entities.each<cmpt::Transform, cmpt::Sprite>([this, view = view, projection = projection](entityx::Entity entity, cmpt::Transform& transform, cmpt::Sprite& sprite) {
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images
        GLCall(glBindTexture(sprite.target, sprite.textureID));

        glm::mat4 mvp = projection * view * getModelMatrix(transform);
        sprite.ib->bind();
        sprite.shader->setUniformMat4f("u_mvp", mvp);
        GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

        // Unbind everything
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GLCall(glBindVertexArray(0));
        GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
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
