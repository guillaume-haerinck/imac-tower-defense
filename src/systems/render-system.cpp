#include "render-system.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "logger/gl-log-handler.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "components/primitive.hpp"
#include "components/health.hpp"
#include "components/health-bar.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/wiggle.hpp"
#include <SDL2/SDL.h>
#include "components/attached-to.hpp"
#include "components/age.hpp"
#include "events/laser-particle-dead.hpp"
#include "core/get-position.hpp"

#include <spdlog/spdlog.h>

RenderSystem::RenderSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, glm::mat4& viewMat, glm::mat4& projMat)
: ISystem(registry, emitter), m_view(viewMat), m_projection(projMat) {}

void RenderSystem::update(float deltatime) {
    /* 
        TODO find a way to use only a few glDraw by sharing buffer or using vertex array. Each draw call should draw all sprites of a particular type. For uniforms, transfer them to vertex attributes
        https://community.khronos.org/t/best-practices-to-render-multiple-2d-sprite-with-vbo/74096
    */

	//Update age
	m_registry.view<cmpt::Age>().each([this,deltatime](auto entity, cmpt::Age & age) {
		age.age += deltatime;
		if (age.lifespan < age.age) {
			m_emitter.publish<evnt::LaserParticleDead>(m_registry.get<cmpt::Transform>(entity).position);
			m_registry.destroy(entity);
		}
	});

    m_registry.view<cmpt::Transform, cmpt::Primitive>().each([this](auto entity, cmpt::Transform& transform, cmpt::Primitive& primitive) {
        // Binding
        primitive.shader->bind();
        GLCall(glBindVertexArray(primitive.vaID));

        // Updates
        glm::mat4 mvp = this->m_projection * this->m_view * this->getModelMatrix(entity);
        primitive.shader->setUniformMat4f("u_mvp", mvp);
        primitive.shader->setUniform4f("u_color", primitive.color.r, primitive.color.g, primitive.color.b, primitive.color.a);
        GLCall(glDrawArrays(primitive.type, 0, primitive.vertexCount));

        // Unbinding
        GLCall(glBindVertexArray(0));
        primitive.shader->unbind();
    });

	m_registry.view<renderTag::Atlas, cmpt::Transform, cmpt::Sprite, cmpt::SpriteAnimation>().each([this](auto entity, auto, cmpt::Transform & transform, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation) {
		// Binding
		sprite.shader->bind();
		GLCall(glBindVertexArray(sprite.vaID));
		GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
		GLCall(glBindTexture(sprite.target, sprite.textureID));
		sprite.ib->bind();

		// Updates
		glm::mat4 mvp = this->m_projection * this->m_view * this->getModelMatrix(entity);
		sprite.shader->setUniformMat4f("u_mvp", mvp);
		sprite.shader->setUniform1i("u_activeTile", animation.activeTile);
		GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

		// Unbinding
		sprite.ib->unbind();
		GLCall(glBindTexture(sprite.target, 0));
		GLCall(glBindVertexArray(0));
		sprite.shader->unbind();
	});

	m_registry.view<renderTag::Single, cmpt::Transform, cmpt::Sprite>().each([this](auto entity, auto, cmpt::Transform & transform, cmpt::Sprite & sprite) {
		// Binding
		sprite.shader->bind();
		GLCall(glBindVertexArray(sprite.vaID));
		GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
		GLCall(glBindTexture(sprite.target, sprite.textureID));
		sprite.ib->bind();

		// Updates
		glm::mat4 mvp = this->m_projection * this->m_view * this->getModelMatrix(entity);
		sprite.shader->setUniformMat4f("u_mvp", mvp);
		GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

		// Unbinding
		sprite.ib->unbind();
		GLCall(glBindTexture(sprite.target, 0));
		GLCall(glBindVertexArray(0));
		sprite.shader->unbind();
	});

	m_registry.view<cmpt::Transform, cmpt::Health, cmpt::HealthBar>().each([this](auto entity, cmpt::Transform & transform, cmpt::Health & health, cmpt::HealthBar & healthbar) {
		if (health.current != health.max) {
			// Background
			{
				// Binding
				healthbar.background.shader->bind();
				GLCall(glBindVertexArray(healthbar.background.vaID));

				// Update pos
				cmpt::Transform healthTransform = transform;
				healthTransform.rotation = 0;
				healthTransform.position += healthbar.relativePos;
				healthTransform.zIndex = Z_INDEX_HUD - 1;

				// Updates
				glm::mat4 mvp = this->m_projection * this->m_view * this->getModelMatrix(healthTransform);
				healthbar.background.shader->setUniformMat4f("u_mvp", mvp);
				healthbar.background.shader->setUniform4f("u_color", healthbar.background.color.r, healthbar.background.color.g, healthbar.background.color.b, healthbar.background.color.a);
				GLCall(glDrawArrays(healthbar.background.type, 0, healthbar.background.vertexCount));

				// Unbinding
				GLCall(glBindVertexArray(0));
				healthbar.background.shader->unbind();
			}

			// Foreground
			{
				// Binding
				healthbar.bar.shader->bind();
				GLCall(glBindVertexArray(healthbar.bar.vaID));

				// Update pos
				cmpt::Transform healthTransform = transform;
				healthTransform.rotation = 0;
				healthTransform.position += healthbar.relativePos;
				float scale = imaths::rangeMapping(health.current, 0, health.max, 0, 1);
				healthTransform.scale = glm::vec2(scale, 1.0f);
				healthTransform.zIndex = Z_INDEX_HUD;

				// Updates
				glm::mat4 mvp = this->m_projection * this->m_view * this->getModelMatrix(healthTransform);
				healthbar.bar.shader->setUniformMat4f("u_mvp", mvp);
				if (scale > 0.4f) {
					healthbar.bar.shader->setUniform4f("u_color", healthbar.bar.color.r, healthbar.bar.color.g, healthbar.bar.color.b, healthbar.bar.color.a);
				}
				else {
					healthbar.bar.shader->setUniform4f("u_color", 1, 0, 0, healthbar.bar.color.a);
				}
				GLCall(glDrawArrays(healthbar.bar.type, 0, healthbar.bar.vertexCount));

				// Unbinding
				GLCall(glBindVertexArray(0));
				healthbar.background.shader->unbind();
			}
		}
	});
}

glm::mat4 RenderSystem::getModelMatrix(unsigned int entityId) const {
	glm::mat4 model(1.0f);
	cmpt::Transform& transform = m_registry.get<cmpt::Transform>(entityId);

	model = glm::translate(model, glm::vec3(getPosition(m_registry,entityId), transform.zIndex));
	model = glm::rotate(model, transform.rotation, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(transform.scale, 0.0f));
    return model;
}

glm::mat4 RenderSystem::getModelMatrix(cmpt::Transform& transform) const {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(transform.position, transform.zIndex));
	model = glm::rotate(model, transform.rotation, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(transform.scale, 0.0f));
	return model;
}