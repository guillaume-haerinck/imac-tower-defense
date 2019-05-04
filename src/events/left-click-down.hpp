#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct LeftClickDown {
		LeftClickDown(glm::vec2 mousePos, glm::vec2 mousePosSdl)
			: mousePos(mousePos), mousePosSdlCoord(mousePosSdl) {}
		LeftClickDown(int mousePosX, int mousePosY, int mousePosSdlX, int mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {}
		LeftClickDown(float mousePosX, float mousePosY, float mousePosSdlX, float mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {}

		glm::vec2 mousePos;
		glm::vec2 mousePosSdlCoord;
	};
}
