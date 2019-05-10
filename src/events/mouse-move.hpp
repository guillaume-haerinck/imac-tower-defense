#pragma once

#include <glm/glm.hpp>

namespace evnt {
	struct MouseMove {
		MouseMove(glm::vec2 mousePos, glm::vec2 mousePosSdl)
			: mousePos(mousePos), mousePosSdlCoord(mousePosSdl) {
		}
		MouseMove(int mousePosX, int mousePosY, int mousePosSdlX, int mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {
		}
		MouseMove(float mousePosX, float mousePosY, float mousePosSdlX, float mousePosSdlY)
			: mousePos(glm::vec2(mousePosX, mousePosY)), mousePosSdlCoord(glm::vec2(mousePosSdlX, mousePosSdlY)) {
		}

		glm::vec2 mousePos;
		glm::vec2 mousePosSdlCoord;
	};
}
