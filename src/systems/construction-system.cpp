#include "construction-system.hpp"

#include <spdlog/spdlog.h>

#include "events/click.hpp"

ConstructionSystem::ConstructionSystem(entt::DefaultRegistry& registry, EventEmitter& emitter)
: System(registry), m_emitter(emitter)
{
	m_emitter.on<evnt::Click>([](const evnt::Click & event, EventEmitter & emitter) {
		spdlog::info("Asynchronious event recieved with value {}", event.mousePos.x);
		/*
		glm::vec2 tilePosition = map1.windowToGrid(e.button.x, (SDL_GetWindowSurface(game.getWindow())->h) - e.button.y);
		unsigned int entityId = map1.getTile(tilePosition.x, tilePosition.y);
		cmpt::Transform trans = registry.get<cmpt::Transform>(entityId);
		enemyFactory.create(trans.position.x, trans.position.y);
		*/
	});
}

void ConstructionSystem::update() {
	
}
