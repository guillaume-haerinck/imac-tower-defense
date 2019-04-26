#pragma once

namespace evnt {
	struct Click {
		Click(glm::vec2 mousePos) : mousePos(mousePos) {}
		Click(int mousePosX, int mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}
		Click(float mousePosX, float mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}

		glm::vec2 mousePos;
	};
}
