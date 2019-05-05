#pragma once

#include <entt/entt.hpp>

#include "i-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include <glm/glm.hpp>

class LaserSystem : public ISystem {
public:
	LaserSystem(entt::DefaultRegistry& registry, EventEmitter& emitter);
    void update(float deltatime) override;
	void connectEvents() override;
	void disconnectEvents() override;

private:
	void shootLaser(glm::vec2 pos, float agl, int nbBounce);
	EventEmitter & m_emitter;
};
