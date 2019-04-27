#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct MouseMove {
		MouseMove(glm::vec2 mousePos) : mousePos(mousePos) {}
		MouseMove(int mousePosX, int mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}
		MouseMove(float mousePosX, float mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}

		glm::vec2 mousePos;
	};
}
