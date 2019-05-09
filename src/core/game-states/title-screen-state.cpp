#include "title-screen-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "core/game.hpp"
#include "logger/gl-log-handler.hpp"

TitleScreenState::TitleScreenState(Game& game) : IGameState(game)//, m_titleScreen(game.emitter)
{
	//m_xaml = m_titleScreen;
	//m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	//m_ui->SetIsPPAAEnabled(true);
	//m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	//m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

TitleScreenState::~TitleScreenState() {
	//m_ui->GetRenderer()->Shutdown();
	//delete m_ui;
}

void TitleScreenState::enter() {
	// Remove subscription for unsused systems
	m_game.animationSystem->disconnectInputs();
	m_game.attackSystem->disconnectInputs();
	m_game.constructionSystem->disconnectInputs();
	m_game.healthSystem->disconnectInputs();
	m_game.movementSystem->disconnectInputs();
	m_game.renderSystem->disconnectInputs();
	m_game.waveSystem->disconnectInputs();

	// Subscribe self to inputs
	connectInputs();
}

void TitleScreenState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT));
	GLCall(glClearStencil(0));

	// Render
	m_ui->GetRenderer()->Render();
}

void TitleScreenState::exit() {
	// Remove input listenner
	disconnectInputs();
}

void TitleScreenState::onLeftClickUp(const evnt::LeftClickUp& event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void TitleScreenState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}