#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

enum FocusMode {
	GAME,
	HUD
};

struct EventEmitter : entt::Emitter<EventEmitter> {
	EventEmitter() : focus(GAME) {}
	FocusMode focus;
	glm::vec2 mousePos;
};
