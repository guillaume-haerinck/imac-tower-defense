#include "construction-system.hpp"

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "events/left-click-up.hpp"
#include "components/transform.hpp"

ConstructionSystem::ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Level& level)
: System(registry), m_emitter(emitter), m_level(level), m_towerFactory(registry)
{
	m_emitter.on<evnt::LeftClickUp>([this](const evnt::LeftClickUp & event, EventEmitter& emitter) {
		glm::vec2 tilePosition = this->m_level.projToGrid(event.mousePos.x, event.mousePos.y);
		unsigned int entityId = this->m_level.getTile(tilePosition.x, tilePosition.y);
		if (entityId != -1) {
			if (this->m_registry.has<tileTag::Constructible>(entityId)) {
				cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(entityId);
				this->m_towerFactory.create(trans.position.x, trans.position.y);
				this->m_registry.remove<tileTag::Constructible>(entityId);
			}
		}
	});
}

