#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct LeftClick {
		LeftClick(glm::vec2 mousePos) : mousePos(mousePos) {}
		LeftClick(int mousePosX, int mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}
		LeftClick(float mousePosX, float mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}

		glm::vec2 mousePos;
	};
}
