#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct RightClickUp {
		RightClickUp(glm::vec2 mousePos, glm::vec2 mousePosSdl)
			: mousePos(mousePos), mousePosSdlCoord(mousePosSdl) {
		}
		RightClickUp(int mousePosX, int mousePosY, int mousePosSdlX, int mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {
		}
		RightClickUp(float mousePosX, float mousePosY, float mousePosSdlX, float mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {
		}

		glm::vec2 mousePos;
		glm::vec2 mousePosSdlCoord;
	};
}
