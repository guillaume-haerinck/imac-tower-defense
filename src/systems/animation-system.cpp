#include "animation-system.hpp"

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "components/transform.hpp"

#include "events/enemy-dead.hpp"
#include "events/tower-dead.hpp"

#include <spdlog/spdlog.h>

AnimationSystem::AnimationSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: ISystem(registry, emitter), m_vfxFactory(registry)
{
	m_emitter.on<evnt::EnnemyDead>([this](const evnt::EnnemyDead & event, EventEmitter & emitter) {
		if (event.type == EnemyType::DRONE) {
			m_vfxFactory.createExplosion(event.position, ENEMY_EXPLOSION);
		}
		if (event.type == EnemyType::KAMIKAZE) {
			m_vfxFactory.createExplosion(event.position+glm::vec2(5,5), ENEMY_EXPLOSION);
			m_vfxFactory.createExplosion(event.position + glm::vec2(-5, 5), ENEMY_EXPLOSION);
			m_vfxFactory.createExplosion(event.position + glm::vec2(5, -5), ENEMY_EXPLOSION);
			m_vfxFactory.createExplosion(event.position + glm::vec2(-5, -5), ENEMY_EXPLOSION);
		}
	});

	m_emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		m_vfxFactory.createExplosion(event.position, TOWER_EXPLOSION);
	});
}

void AnimationSystem::update(float deltatime) {
    m_registry.view<cmpt::SpriteAnimation, cmpt::Sprite>().each([this,deltatime](auto entity, cmpt::SpriteAnimation& animation, cmpt::Sprite& sprite) {
		animation.age += deltatime;
        if (animation.age < animation.duration) {
            animation.activeTile = animation.startTile + animation.age/animation.duration*(animation.endTile-animation.startTile) ;
        }
		else {
			if (m_registry.has<renderTag::OneTimeAtlas>(entity)) {
				m_registry.destroy(entity);
			}
			else {
				animation.activeTile = animation.startTile;
				animation.age = 0;
			}
        }
    });
}
