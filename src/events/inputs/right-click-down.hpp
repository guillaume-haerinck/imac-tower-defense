#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct RightClickDown {
		RightClickDown(glm::vec2 mousePos, glm::vec2 mousePosSdl)
			: mousePos(mousePos), mousePosSdlCoord(mousePosSdl) {}
		RightClickDown(int mousePosX, int mousePosY, int mousePosSdlX, int mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {}
		RightClickDown(float mousePosX, float mousePosY, float mousePosSdlX, float mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {}

		glm::vec2 mousePos;
		glm::vec2 mousePosSdlCoord;
	};
}
