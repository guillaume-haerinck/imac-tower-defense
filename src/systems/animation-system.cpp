#include "animation-system.hpp"

#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "logger/gl-log-handler.hpp"

AnimationSystem::AnimationSystem() {}

AnimationSystem::~AnimationSystem() {}

void AnimationSystem::update(entityx::EntityX& registry, double deltatime) {
    registry.entities.each<cmpt::SpriteAnimation, cmpt::Sprite>([](entityx::Entity entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
        /*
        if (animation.activeTile < animation.endTile) {
            animation.activeTile++;
        } else {
            animation.activeTile = animation.startTile;
        }
        
        sprite.shader->bind();
        GLCall(glBindVertexArray(sprite.vaID));
        GLCall(glActiveTexture(GL_TEXTURE0)); // Texture unit 0 for images
        GLCall(glBindTexture(sprite.target, sprite.textureID));
        sprite.ib->bind();
        sprite.shader->setUniform1i("u_activeTile", animation.activeTile);
        */
    });
}
