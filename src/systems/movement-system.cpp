#include "movement-system.hpp"

#include "components/transform.hpp"
#include "core/tags.hpp"

MovementSystem::MovementSystem() {}

MovementSystem::~MovementSystem() {}

void MovementSystem::update(entt::registry<>& registry, double deltatime) {
   registry.group<tag::Minutes>(entt::get<cmpt::Transform>).each([](auto entity, cmpt::Transform &transform) {
       transform.rotation = glm::quat(glm::vec3(0.0f, 0.0f, 1.0f));
   });
}
