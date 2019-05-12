#include <entt/entt.hpp>

#include "debug-draw/i-debug-draw.hpp"
#include "audio/i-audio.hpp"
#include "random/i-random.hpp"
#include "get-position/i-get-position.hpp"

// Is init in the constructor of Game class in core folder
struct locator {
	using debugDraw = entt::ServiceLocator<IDebugDraw>;
	using audio = entt::ServiceLocator<IAudio>;
	using random = entt::ServiceLocator<IRandom>;
	using getPosition = entt::ServiceLocator<IGetPosition>;
};
