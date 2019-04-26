#pragma once

namespace evnt {
	struct Move {
		Move(glm::vec2 mousePos) : mousePos(mousePos) {}
		Move(int mousePosX, int mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}
		Move(float mousePosX, float mousePosY) : mousePos(glm::vec2(mousePosX, mousePosY)) {}

		glm::vec2 mousePos;
	};
}
