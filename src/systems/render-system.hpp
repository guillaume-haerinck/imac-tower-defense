#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "i-system.hpp"
#include "components/transform.hpp"

/* TODO use framebuffer */

class RenderSystem : public ISystem {
public:
    RenderSystem(entt::DefaultRegistry& registry, glm::mat4& viewMat, glm::mat4& projMat);
    void update(float deltatime) override;
	void connectEvents() override;
	void disconnectEvents() override;

private:
    glm::mat4 getModelMatrix(cmpt::Transform& transform) const;
	const glm::mat4& m_view;
	const glm::mat4& m_projection;
};
