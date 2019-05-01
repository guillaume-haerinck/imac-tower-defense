#pragma once

#include <glm/glm.hpp>

#include "primitive.hpp"

namespace cmpt {
	struct HealthBar {
		HealthBar(glm::vec2 relativePos, Primitive background, Primitive bar) : relativePos(relativePos), background(background), bar(bar) {}

		glm::vec2 relativePos;
		Primitive background;
		Primitive bar;
	};
}