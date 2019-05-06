#include "mirror-factory.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "core/tags.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"

#include "services/locator.hpp"
#include "services/random/i-random.hpp"

MirrorFactory::MirrorFactory(entt::DefaultRegistry& registry) : Factory(registry)
{
	m_mirrorSprite = m_spriteFactory.createSingle("res/images/textures/missing.png", glm::vec2(8.0f));
}

MirrorFactory::~MirrorFactory() {
	GLCall(glDeleteTextures(1, &m_mirrorSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_mirrorSprite.vaID));
}

void MirrorFactory::create(float posX, float posY) {
	auto myEntity = m_registry.create();
	m_registry.assign<entityTag::Mirror>(myEntity);
	m_registry.assign<cmpt::Sprite>(myEntity, m_mirrorSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY));
}
