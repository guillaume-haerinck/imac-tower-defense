#pragma once

#include <entt/entt.hpp>

#include "system.hpp"
#include "events/handlers/event-emitter.hpp"
#include <glm/glm.hpp>

class LaserSystem : public System {
public:
	LaserSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(double deltatime);

private:
	void shootLaser(glm::vec2 pos, float agl, int nbBounce);
	EventEmitter & m_emitter;
};
