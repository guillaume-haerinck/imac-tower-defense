#include "animation-system.hpp"

#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"

AnimationSystem::AnimationSystem() {}

AnimationSystem::~AnimationSystem() {}

void AnimationSystem::update(entt::registry<>& registry, double deltatime) {
    registry.view<cmpt::SpriteAnimation, cmpt::Sprite>().each([&](auto entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
        if (animation.activeTile < animation.endTile) {
            animation.activeTile++;
        } else {
            animation.activeTile = animation.startTile;
        }
        sprite.shader->bind();
        sprite.shader->setUniform1i("u_activeTile", animation.activeTile);
    });
}
