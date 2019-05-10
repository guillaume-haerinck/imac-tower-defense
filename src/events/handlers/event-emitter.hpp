#pragma once

#include <entt/entt.hpp>

enum FocusMode {
	GAME,
	HUD
};

struct EventEmitter : entt::Emitter<EventEmitter> {
	EventEmitter() : focus(GAME) {}
	FocusMode focus;
};
