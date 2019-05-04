#include "construction-system.hpp"

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/constants.hpp"
#include "core/tags.hpp"
#include "events/left-click.hpp"
#include "components/transform.hpp"

ConstructionSystem::ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, Map& map)
: System(registry), m_emitter(emitter), m_map(map), m_towerFactory(registry)
{
	m_emitter.on<evnt::LeftClick>([this](const evnt::LeftClick & event, EventEmitter& emitter) {
		glm::vec2 tilePosition = this->m_map.projToGrid(event.mousePos.x, event.mousePos.y);
		unsigned int entityId = this->m_map.getTile(tilePosition.x, tilePosition.y);
		if (entityId != -1) {
			if (this->m_registry.has<tileTag::Constructible>(entityId)) {
				cmpt::Transform trans = this->m_registry.get<cmpt::Transform>(entityId);
				this->m_towerFactory.create(trans.position.x, trans.position.y);
				this->m_registry.remove<tileTag::Constructible>(entityId);
			}
		}
	});
}

