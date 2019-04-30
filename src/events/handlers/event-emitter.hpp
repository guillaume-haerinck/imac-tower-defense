#pragma once

#include <entt/entt.hpp>

struct EventEmitter : entt::Emitter<EventEmitter> {};

// TODO use another struct, an event dispatcher for physic, because it is synchronious
