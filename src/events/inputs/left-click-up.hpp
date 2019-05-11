#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct LeftClickUp {
		LeftClickUp(glm::vec2 mousePos, glm::vec2 mousePosSdl)
			: mousePos(mousePos), mousePosSdlCoord(mousePosSdl) {
		}
		LeftClickUp(int mousePosX, int mousePosY, int mousePosSdlX, int mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {
		}
		LeftClickUp(float mousePosX, float mousePosY, float mousePosSdlX, float mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {
		}

		glm::vec2 mousePos;
		glm::vec2 mousePosSdlCoord;
	};
}
