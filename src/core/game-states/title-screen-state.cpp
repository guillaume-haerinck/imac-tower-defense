#include "title-screen-state.hpp"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "core/game.hpp"

TitleScreenState::TitleScreenState(Game& game) : IGameState(game), m_titleScreen(game.emitter)
{
	m_xaml = m_titleScreen;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(game.noesisDevice);
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

TitleScreenState::~TitleScreenState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void TitleScreenState::enter() {
	// Subscribe self to inputs
	connectInputs();
}

void TitleScreenState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks() / 1000.0f);
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

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

void TitleScreenState::onMouseMove(const evnt::MouseMove& event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);
	m_game.emitter.mousePos = event.mousePos;
}
