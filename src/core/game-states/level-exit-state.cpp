#include "level-exit-state.hpp"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "core/game.hpp"

LevelExitState::LevelExitState(Game& game) : IGameState(game), m_levelExit(game.emitter)
{
	m_xaml = m_levelExit;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

LevelExitState::~LevelExitState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelExitState::enter() {
	connectInputs();
}

void LevelExitState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks() / 1000.0f);
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

	// Render
	m_ui->GetRenderer()->Render();
}

void LevelExitState::exit() {
	disconnectInputs();
}

void LevelExitState::onLeftClickUp(const evnt::LeftClickUp& event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void LevelExitState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void LevelExitState::onMouseMove(const evnt::MouseMove& event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);
	m_game.emitter.mousePos = event.mousePos;
}
