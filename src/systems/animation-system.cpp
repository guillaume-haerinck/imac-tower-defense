#include "animation-system.hpp"

#include "core/constants.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"

AnimationSystem::AnimationSystem(entt::DefaultRegistry& registry) : System(registry) {}

void AnimationSystem::update(double deltatime) {
    m_registry.view<cmpt::SpriteAnimation, cmpt::Sprite>().each([](auto entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
        if (animation.activeTile < animation.endTile) {
            animation.activeTile++;
        } else {
            animation.activeTile = animation.startTile;
        }
    });
}
