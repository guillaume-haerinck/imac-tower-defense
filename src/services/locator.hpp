#include <entt/entt.hpp>

#include "debug-draw/i-debug-draw.hpp"
#include "audio/i-audio.hpp"

// Is init in the constructor of Game class in core folder
struct locator {
	using debugDraw = entt::ServiceLocator<IDebugDraw>;
	using audio = entt::ServiceLocator<IAudio>;
};
