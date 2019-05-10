#pragma once

#include <entt/entt.hpp>

enum FocusMode {
	HUD,
	GAME
};

struct EventEmitter : entt::Emitter<EventEmitter> {
	FocusMode focus;
};

// TODO use another struct, an event dispatcher for physic, because it is synchronious
