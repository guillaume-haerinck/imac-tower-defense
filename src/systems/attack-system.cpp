#include "attack-system.hpp"

#include <math.h>
#include <spdlog/spdlog.h>

#include "core/tags.hpp"
#include "components/targeting.hpp"
#include "components/look-at.hpp"
#include "components/shoot-at.hpp"
#include "components/trigger.hpp"
#include "components/transform.hpp"

#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"

AttackSystem::AttackSystem(entt::DefaultRegistry& registry) : System(registry), m_projectileFactory(registry) {}

void AttackSystem::update() {

	m_registry.view<cmpt::Targeting, cmpt::Transform , cmpt::LookAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, cmpt::LookAt lookAt) {
		if ( m_registry.valid(targeting.targetId) ) {
			glm::vec2 direction = m_registry.get<cmpt::Transform>(targeting.targetId).position - transform.position;
			transform.rotation = atan2(direction.y, direction.x);
		}
	});

	m_registry.view<cmpt::Targeting, cmpt::Transform, cmpt::ShootAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, cmpt::ShootAt & shootAt) {
		if (m_registry.valid(targeting.targetId) ) {
			if (shootAt.timer == shootAt.loadingTime) {
				m_projectileFactory.create(transform.position, targeting.targetId);
				shootAt.timer = 0;
			}
			else {
				shootAt.timer++;
			}
		}
	});

	m_registry.view<cmpt::Transform, cmpt::Trigger, entityTag::Tower>().each([this](auto entity1, cmpt::Transform & transform1, cmpt::Trigger & trigger1, auto) {
		IDebugDraw& dd = entt::ServiceLocator<IDebugDraw>::ref();
		dd.DrawCircle(b2Vec2(transform1.position.x, transform1.position.y), trigger1.radius, b2Color(1, 0, 0, 0.5f));

		m_registry.view<cmpt::Transform, cmpt::Trigger, entityTag::Enemy>().each([this, entity1, transform1, trigger1](auto entity2, cmpt::Transform & transform2, cmpt::Trigger & trigger2, auto) {
			if (entity1 != entity2) {
				const glm::vec2 pos = transform1.position - transform2.position;
				const float distance = sqrt(pos.x * pos.x + pos.y * pos.y);
				
				if (distance <= trigger1.radius + trigger2.radius) {
					spdlog::info("trigger entered !");
				}
			}
		});
	});
}