#include "end-screen-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "core/game.hpp"

EndScreenState::EndScreenState(Game& game) : IGameState(game), m_endScreen(m_game.emitter) {
	m_xaml = m_endScreen;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

EndScreenState::~EndScreenState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void EndScreenState::enter() {
	// Subscribe to events
	connectInputs();
}

void EndScreenState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks() / 1000.0f);
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

	// Render
	m_ui->GetRenderer()->Render();
}

void EndScreenState::exit() {
	// Unsubscribe
	disconnectInputs();
}


void EndScreenState::onLeftClickUp(const evnt::LeftClickUp & event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void EndScreenState::onLeftClickDown(const evnt::LeftClickDown & event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void EndScreenState::onMouseMove(const evnt::MouseMove & event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);
	m_game.emitter.mousePos = event.mousePos;
}
