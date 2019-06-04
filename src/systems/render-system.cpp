#include "render-system.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

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
#include "components/attached-to.hpp"
#include "components/age.hpp"
#include "components/shake.hpp"
#include "components/tint-colour.hpp"
#include "components/animated.hpp"
#include "components/animation-pixels-vanish.hpp"
#include "components/growing-circle.hpp"
#include "events/laser-particle-dead.hpp"
#include "events/entity-damaged.hpp"

#include "services/locator.hpp"
#include "services/helper/i-helper.hpp"

#include <spdlog/spdlog.h>

RenderSystem::RenderSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, glm::mat4& viewMat, glm::mat4& projMat)
: ISystem(registry, emitter), m_view(viewMat), m_projection(projMat) {
	m_emitter.on<evnt::EntityDamaged>([this](const evnt::EntityDamaged & event, EventEmitter & emitter) {
		if (m_registry.valid(event.entity)) {
			//Shake
			if (m_registry.has<cmpt::Shake>(event.entity)) {
				IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
				float agl = randomService.random(imaths::TAU);
				m_registry.get<cmpt::Shake>(event.entity).offset = 0.3f*glm::vec2(cos(agl), sin(agl));
			}
		}
	});

	initCursors();
	m_emitter.on<evnt::ChangeCursor>([this](const evnt::ChangeCursor & event, EventEmitter & emitter) {
		SDL_SetCursor(this->m_cursors.at(event.cursor));
	});
}

void RenderSystem::initCursors() {
	m_cursors.at(CursorType::ARROW) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	m_cursors.at(CursorType::CLICK) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	m_cursors.at(CursorType::LOADING) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	m_cursors.at(CursorType::NO) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
	m_cursors.at(CursorType::ROTATION) = createCustomCursor("res/images/cursors/rotate.png");
	m_cursors.at(CursorType::ACTIVATE) = createCustomCursor("res/images/cursors/on.png");
	m_cursors.at(CursorType::DESACTIVATE) = createCustomCursor("res/images/cursors/off.png");
}

SDL_Cursor* RenderSystem::createCustomCursor(std::string imagePath) {
	int req_format = STBI_rgb_alpha;
	int width, height, orig_format;
	unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &orig_format, req_format);
	if (data == nullptr) {
		spdlog::warn("[Cursor] image invalide ! {}", imagePath);
		debug_break();
	}
	m_cursorImages.push_back(data);

	// Set up the pixel format color masks for RGB(A) byte arrays.
	// Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
	Uint32 rmask, gmask, bmask, amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		int shift = (req_format == STBI_rgb) ? 8 : 0;
		rmask = 0xff000000 >> shift;
		gmask = 0x00ff0000 >> shift;
		bmask = 0x0000ff00 >> shift;
		amask = 0x000000ff >> shift;
	#else // little endian, like x86
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = (req_format == STBI_rgb) ? 0 : 0xff000000;
	#endif

	int depth, pitch;
	if (req_format == STBI_rgb) {
		depth = 24;
		pitch = 3 * width; // 3 bytes per pixel * pixels per row
	} else { // STBI_rgb_alpha (RGBA)
		depth = 32;
		pitch = 4 * width;
	}

	SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*) data, width, height, depth, pitch, rmask, gmask, bmask, amask);
	if (surf == nullptr) {
		spdlog::warn("[Cursor] impossible de créer la surface ! {}", SDL_GetError());
		debug_break();
	}
	m_cursorSurfaces.push_back(surf);
	return SDL_CreateColorCursor(surf, width / 2, height / 2);
}

RenderSystem::~RenderSystem() {
	for (int i = 0; i < m_cursors.size(); i++) {
		SDL_FreeCursor(m_cursors.at(i));
	}

	for (int i = 0; i < m_cursorSurfaces.size(); i++) {
		SDL_FreeSurface(m_cursorSurfaces.at(i));
	}

	for (int i = 0; i < m_cursorImages.size(); i++) {
		stbi_image_free(m_cursorImages.at(i));
	}
}

void RenderSystem::renderSpritesheet(std::uint32_t entity, cmpt::Sprite& sprite, cmpt::SpriteAnimation& animation) const {
	IHelper& helper = entt::ServiceLocator<IHelper>::ref();
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
	if (m_registry.valid(entity)) {
		sprite.shader->setUniform4f("tintColour", helper.getColour(entity));
		sprite.shader->setUniform1f("u_alpha", helper.getAlpha(entity));
		if (m_registry.has<cmpt::AnimationPixelsVanish>(entity) || (m_registry.has<cmpt::AttachedTo>(entity) && m_registry.has<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity))) {
			cmpt::Animated animated = cmpt::Animated(0);
			bool bForward;
			if (m_registry.has<cmpt::AnimationPixelsVanish>(entity)) {
				animated = m_registry.get<cmpt::Animated>(entity);
				bForward = m_registry.get<cmpt::AnimationPixelsVanish>(entity).bForward;
			}
			else {
				animated = m_registry.get<cmpt::Animated>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity);
				bForward = m_registry.get<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity).bForward;
			}
			if (bForward) {
				sprite.shader->setUniform1f("probaDisappear", 1 - animated.age / animated.duration);
			}
			else {
				sprite.shader->setUniform1f("probaDisappear", animated.age / animated.duration);
			}
		}
		else {
			sprite.shader->setUniform1f("probaDisappear", 0);
		}
	}
	GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

	// Unbinding
	sprite.ib->unbind();
	GLCall(glBindTexture(sprite.target, 0));
	GLCall(glBindVertexArray(0));
	sprite.shader->unbind();
}

void RenderSystem::renderSprite(std::uint32_t entity, cmpt::Sprite & sprite) const {
	IHelper& helper = entt::ServiceLocator<IHelper>::ref();
	// Binding
	sprite.shader->bind();
	GLCall(glBindVertexArray(sprite.vaID));
	GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images, must be called before binding texture
	GLCall(glBindTexture(sprite.target, sprite.textureID));
	sprite.ib->bind();

	// Updates
	glm::mat4 mvp = m_projection * m_view * getModelMatrix(entity);
	sprite.shader->setUniformMat4f("u_mvp", mvp);
	if (m_registry.valid(entity)) {
		sprite.shader->setUniform4f("tintColour", helper.getColour(entity));
		sprite.shader->setUniform1f("u_alpha", helper.getAlpha(entity));
		if (m_registry.has<cmpt::AnimationPixelsVanish>(entity) || (m_registry.has<cmpt::AttachedTo>(entity) && m_registry.has<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity))) {
			cmpt::Animated animated = cmpt::Animated(0);
			bool bForward;
			if (m_registry.has<cmpt::AnimationPixelsVanish>(entity)) {
				animated = m_registry.get<cmpt::Animated>(entity);
				bForward = m_registry.get<cmpt::AnimationPixelsVanish>(entity).bForward;
			}
			else {
				animated = m_registry.get<cmpt::Animated>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity);
				bForward = m_registry.get<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity).bForward;
			}
			if (bForward) {
				sprite.shader->setUniform1f("probaDisappear", 1 - animated.age / animated.duration);
			}
			else {
				sprite.shader->setUniform1f("probaDisappear", animated.age / animated.duration);
			}
		}
		else {
			sprite.shader->setUniform1f("probaDisappear", 0);
		}
	}
	GLCall(glDrawElements(GL_TRIANGLES, sprite.ib->getCount(), GL_UNSIGNED_INT, nullptr));

	// Unbinding
	sprite.ib->unbind();
	GLCall(glBindTexture(sprite.target, 0));
	GLCall(glBindVertexArray(0));
	sprite.shader->unbind();
}

void RenderSystem::update(float deltatime) {
    /* 
        TODO find a way to use only a few glDraw by sharing buffer or using vertex array. Each draw call should draw all sprites of a particular type. For uniforms, transfer them to vertex attributes
        https://community.khronos.org/t/best-practices-to-render-multiple-2d-sprite-with-vbo/74096
    */
    m_registry.view<cmpt::Transform, cmpt::Primitive>().each([this](auto entity, cmpt::Transform& transform, cmpt::Primitive& primitive) {
        // Binding
        primitive.shader->bind();
        GLCall(glBindVertexArray(primitive.vaID));

        // Updates
        glm::mat4 mvp = this->m_projection * this->m_view * this->getModelMatrix(entity);
        primitive.shader->setUniformMat4f("u_mvp", mvp);
        primitive.shader->setUniform4f("u_color", primitive.color.r, primitive.color.g, primitive.color.b, primitive.color.a);
		if (m_registry.valid(entity) && m_registry.has<cmpt::TintColour>(entity)) {
			cmpt::TintColour& tint = m_registry.get<cmpt::TintColour>(entity);
			primitive.shader->setUniform4f("tintColour", tint.col.r, tint.col.g, tint.col.b, tint.col.a);
			if (tint.bOneTimeOnly) {
				m_registry.remove<cmpt::TintColour>(entity);
			}
		}
		else {
			primitive.shader->setUniform4f("tintColour", 0, 0, 0, 0);
		}
        GLCall(glDrawArrays(primitive.type, 0, primitive.vertexCount));

        // Unbinding
        GLCall(glBindVertexArray(0));
        primitive.shader->unbind();
    });

	//Render tiles
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Tile>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Tile>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Highlight tile under mouse while an entity is being placed
	if (m_emitter.entityBeingPlaced) {
		IHelper& helper = entt::ServiceLocator<IHelper>::ref();
		IDebugDraw& debugDraw = entt::ServiceLocator<IDebugDraw>::ref();
		std::uint32_t tile = helper.getTileFromProjCoord(m_emitter.mousePos.x, m_emitter.mousePos.y);
		if (m_registry.has<tileTag::Constructible>(tile)) {
			debugDraw.setColor(glm::vec4(0, 255, 0, 0.3));
		}
		else {
			debugDraw.setColor(glm::vec4(255, 0, 0, 0.3));
		}
		glm::vec2 tilePos = m_registry.get<cmpt::Transform>(tile).position;
		debugDraw.rect(tilePos.x-TILE_SIZE/2, tilePos.y-TILE_SIZE/2, tilePos.x+TILE_SIZE/2, tilePos.y+TILE_SIZE/2, Z_INDEX_TOWER);
	}
	//Render enemies
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Enemy>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Enemy>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Enemy2>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Enemy2>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Render buildings
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Building>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Building>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Render projectiles
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Projectile>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Projectile>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Render VFX
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_VFX>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_VFX>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});

	//Healthbars
	m_registry.view<cmpt::Transform, cmpt::Health, cmpt::HealthBar>().each([this](auto entity, cmpt::Transform & transform, cmpt::Health & health, cmpt::HealthBar & healthbar) {
		if (health.current != health.max && !m_registry.has<cmpt::Animated>(entity)) {
			IHelper& helper = entt::ServiceLocator<IHelper>::ref();
			// Background
			{
				// Binding
				healthbar.background.shader->bind();
				GLCall(glBindVertexArray(healthbar.background.vaID));

				// Update pos
				cmpt::Transform healthTransform(helper.getPosition(entity), Z_INDEX_HUD - 1);
				healthTransform.position += healthbar.relativePos;

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
				cmpt::Transform healthTransform(helper.getPosition(entity), Z_INDEX_HUD);
				healthTransform.position += healthbar.relativePos;
				float scale = imaths::rangeMapping(health.current, 0, health.max, 0, 1);
				healthTransform.scale = glm::vec2(scale, 1.0f);

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

	//Growing kamikaze explosion
	m_registry.view<cmpt::GrowingCircle, cmpt::Age, cmpt::Transform, renderOrderTag::o_VFX>().each([this](auto entity, cmpt::GrowingCircle& growingCircle, cmpt::Age& age, cmpt::Transform& transform, auto) {
		IDebugDraw& debugDraw = entt::ServiceLocator<IDebugDraw>::ref();
		float r = growingCircle.growthSpeed * age.age;
		debugDraw.circleExplosion(transform.position.x, transform.position.y, r, growingCircle.maxRadius);
	});
}

glm::mat4 RenderSystem::getModelMatrix(unsigned int entityId) const {
	IHelper& helper = entt::ServiceLocator<IHelper>::ref();
	glm::mat4 model(1.0f);
	cmpt::Transform& transform = m_registry.get<cmpt::Transform>(entityId);
	model = glm::translate(model, glm::vec3(helper.getPosition(entityId), transform.zIndex));
	if (!m_registry.has<entityTag::Tower>(entityId) && !m_registry.has<entityTag::Mirror>(entityId)) {
		model = glm::rotate(model, transform.rotation, glm::vec3(0, 0, 1));
	}
	model = glm::scale(model, glm::vec3(helper.getScale(entityId), 0.0f));
    return model;
}

glm::mat4 RenderSystem::getModelMatrix(cmpt::Transform& transform) const {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(transform.position, transform.zIndex));
	model = glm::rotate(model, transform.rotation, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(transform.scale, 0.0f));
	return model;
}