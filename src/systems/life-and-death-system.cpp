#include "life-and-death-system.hpp"

#include <spdlog/spdlog.h>

#include "events/entity-damaged.hpp"
#include "events/enemy-dead.hpp"
#include "events/tower-dead.hpp"
#include "events/enemy-reached-end.hpp"
#include "events/victory-delay-ends.hpp"
#include "components/health.hpp"
#include "components/transform.hpp"
#include "components/entity-explosion-association.hpp"
#include "components/growing-circle.hpp"
#include "components/shake.hpp"
#include "components/attached-to.hpp"
#include "components/age.hpp"
#include "components/animated.hpp"
#include "components/animation-dark.hpp"
#include "components/animation-alpha.hpp"
#include "components/animation-scale.hpp"
#include "components/animation-pixels-vanish.hpp"
#include "core/tags.hpp"
#include "core/constants.hpp"
#include "core/maths.hpp"
#include "entity-factories/enemy-factory.hpp"

#include "services/locator.hpp"
#include "services/helper/i-helper.hpp"

LifeAndDeathSystem::LifeAndDeathSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Progression& progression)
: ISystem(registry, emitter), m_progression(progression)
{
	m_emitter.on<evnt::EntityDamaged>([this](const evnt::EntityDamaged & event, EventEmitter & emitter) {
		cmpt::Health& health = m_registry.get<cmpt::Health>(event.entity);
		health.current -= event.damage;
	});

	m_emitter.on<evnt::EnemyReachedEnd>([this](const evnt::EnemyReachedEnd& event, EventEmitter & emitter) {
		m_progression.reduceLifeBy(ENEMY_SURVIVE_LIFE_COST);
		IHelper& helper = entt::ServiceLocator<IHelper>::ref();
		helper.startScreenShake(SCREEN_SHAKE_DURATION);
	});
}

void LifeAndDeathSystem::update(float deltatime) {
	//Update age
	m_registry.view<cmpt::Age>().each([this, deltatime](auto entity, cmpt::Age & age) {
		age.age += deltatime;
		//Destroy entities that have reached their max lifespan
		if (age.age > age.lifespan) {
			if (m_registry.has<entityTag::VictoryDelayTimer>(entity)) {
				m_emitter.publish<evnt::VictoryDelayEnds>();
			}
			m_registry.destroy(entity);
		}
	});
	//Update animation's age
	m_registry.view<cmpt::Animated>().each([this, deltatime](auto entity, cmpt::Animated & animated) {
		animated.age += deltatime;
		//Remove animation once max duration has been reached
		if (animated.age > animated.duration) {
			bool destroyEntity = animated.bDestroyAfterAnimation;
			m_registry.remove<cmpt::Animated>(entity);
			m_registry.reset<cmpt::AnimationScale>(entity);
			m_registry.reset<cmpt::AnimationDark>(entity);
			m_registry.reset<cmpt::AnimationPixelsVanish>(entity);
			m_registry.reset<cmpt::AnimationAlpha>(entity);
			//Destroy entity if it was a disappear animation
			if (destroyEntity) {
				m_registry.destroy(entity);
			}
		}
	});
	//Destroy entities that have no more health
	m_registry.view<cmpt::Health>().each([this](auto entity, cmpt::Health& health) {
		if (health.current <= 0.0001f) {
			IHelper& helper = entt::ServiceLocator<IHelper>::ref();
			//Enemy
			if (m_registry.has<entityTag::Enemy>(entity)) {
				if (m_registry.has<enemyTag::Robot>(entity)) {
					m_emitter.publish<evnt::EnemyDead>(helper.getPosition(entity), EnemyType::ROBOT);
				}
				else if (m_registry.has<enemyTag::Kamikaze>(entity)) {
					m_emitter.publish<evnt::EnemyDead>(helper.getPosition(entity), EnemyType::KAMIKAZE);
				}
			}
			//Tower
			else if (m_registry.has<entityTag::Tower>(entity)) {
				m_emitter.publish<evnt::TowerDead>(helper.getPosition(entity));
			}
			m_registry.destroy(entity);
		}
	});

	//Check if a global explosion has reached an entity
	m_registry.view<cmpt::EntityExplosionAssociation>().each([this](auto entity, cmpt::EntityExplosionAssociation& assoc) {
		if (m_registry.valid(assoc.explosion) && m_registry.valid(assoc.entity)) {
			cmpt::GrowingCircle circle = m_registry.get<cmpt::GrowingCircle>(assoc.explosion);
			float r = circle.growthSpeed * m_registry.get<cmpt::Age>(assoc.explosion).age;
			if (imaths::distanceSq(m_registry.get<cmpt::Transform>(assoc.entity).position, m_registry.get<cmpt::Transform>(assoc.explosion).position) < r*r) {
				m_emitter.publish<evnt::EntityDamaged>(assoc.entity, KAMIKAZE_EXPLOSION_DAMAGE);
				m_registry.destroy(entity);
			}
		}
		else {
			m_registry.destroy(entity);
		}
	});

	//Should be updated last
	//Destroy entities who's main entity was destroyed
	m_registry.view<cmpt::AttachedTo>().each([this](auto entity, cmpt::AttachedTo& attachedTo) {
		if (m_registry.has<cmpt::AttachedTo>(entity)) {
			unsigned int mainEntity = m_registry.get<cmpt::AttachedTo>(entity).mainEntity;
			if (!m_registry.valid(mainEntity)) {
				m_registry.destroy(entity);
			}
		}
	});
}
