#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

enum FocusMode {
	GAME,
	HUD
};

struct EventEmitter : entt::Emitter<EventEmitter> {
	EventEmitter() : focus(GAME) , entityBeingPlaced(false) {}
	FocusMode focus;
	glm::vec2 mousePos;
	bool entityBeingPlaced;
};
