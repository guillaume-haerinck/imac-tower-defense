#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct TranslateView {
		TranslateView(glm::vec2 distance) : distance(distance) {}
		TranslateView(int distanceX, int distanceY) : distance(glm::vec2(distanceX, distanceY)) {}
		TranslateView(float distanceX, float distanceY) : distance(glm::vec2(distanceX, distanceY)) {}

		glm::vec2 distance;
	};
}
