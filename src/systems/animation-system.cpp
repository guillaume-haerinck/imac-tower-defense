#include "animation-system.hpp"

#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"

AnimationSystem::AnimationSystem() {}

AnimationSystem::~AnimationSystem() {}

void AnimationSystem::update(entt::DefaultRegistry& registry, double deltatime) {
    registry.view<cmpt::SpriteAnimation, cmpt::Sprite>().each([](uint32_t entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
        if (animation.activeTile < animation.endTile) {
            animation.activeTile++;
        } else {
            animation.activeTile = animation.startTile;
        }
    });
}
