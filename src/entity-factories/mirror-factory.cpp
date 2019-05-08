#include "mirror-factory.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "core/tags.hpp"
#include "logger/gl-log-handler.hpp"
#include "components/transform.hpp"
#include "components/hitbox.hpp"

#include "services/locator.hpp"
#include "services/random/i-random.hpp"

MirrorFactory::MirrorFactory(entt::DefaultRegistry& registry) : Factory(registry)
{
	m_mirrorSprite = m_spriteFactory.createSingle("res/images/textures/mirror_tmp.png", glm::vec2(8.0f));
}

MirrorFactory::~MirrorFactory() {
	GLCall(glDeleteTextures(1, &m_mirrorSprite.textureID));
	GLCall(glDeleteVertexArrays(1, &m_mirrorSprite.vaID));
}

unsigned int MirrorFactory::create(float posX, float posY) {
	IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
	auto myEntity = m_registry.create();
	m_registry.assign<entityTag::Mirror>(myEntity);
	m_registry.assign<cmpt::Sprite>(myEntity, m_mirrorSprite);
	m_registry.assign<renderTag::Single>(myEntity);
	m_registry.assign<cmpt::Transform>(myEntity, glm::vec2(posX, posY), glm::vec2(1.0f) , randomService.random(6.28));
	m_registry.assign<cmpt::Hitbox>(myEntity, 4.0f);
	return myEntity;
}
