#include "focus-system.hpp"

#include "components/targeting.hpp"
#include "components/look-at.hpp"
#include "components/shoot-at.hpp"
#include "components/transform.hpp"

FocusSystem::FocusSystem(entt::DefaultRegistry& registry) : System(registry), m_projectileFactory(registry) {}

void FocusSystem::update() {

	m_registry.view<cmpt::Targeting, cmpt::Transform , cmpt::LookAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, cmpt::LookAt lookAt) {
		if (targeting.targetId != 0) {
			glm::vec2 direction = m_registry.get<cmpt::Transform>(targeting.targetId).position - transform.position;
			transform.rotation = atan2(direction.y, direction.x);
		}
	});

	m_registry.view<cmpt::Targeting, cmpt::Transform, cmpt::ShootAt>().each([this](auto entity, cmpt::Targeting & targeting, cmpt::Transform & transform, cmpt::ShootAt & shootAt) {
		if (targeting.targetId != 0) {
			if (shootAt.timer == shootAt.loadingTime) {
				m_projectileFactory.create(transform.position, targeting.targetId);
				shootAt.timer = 0;
			}
			else {
				shootAt.timer++;
			}
		}
	});
}