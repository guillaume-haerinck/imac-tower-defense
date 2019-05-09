#include "i-game-state.hpp"

#include <glad/glad.h>

#include "logger/gl-log-handler.hpp"
#include "core/constants.hpp"
#include "core/game.hpp"

IGameState::IGameState(Game& game) : InputHandler(game.emitter), m_game(game) {}

void IGameState::restoreGpuState() {
	// Framebuffer
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT));
	GLCall(glClearStencil(0));

	// Z-buffer
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));
	GLCall(glDepthMask(GL_TRUE));

	// Clear window
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
