#include "animation-system.hpp"

#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"

AnimationSystem::AnimationSystem() {}

AnimationSystem::~AnimationSystem() {}

void AnimationSystem::update(entityx::EntityX& registry, double deltatime) {
    registry.entities.each<cmpt::SpriteAnimation, cmpt::Sprite>([](entityx::Entity entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
        if (animation.activeTile < animation.endTile) {
            animation.activeTile++;
        } else {
            animation.activeTile = animation.startTile;
        }
    });
}
