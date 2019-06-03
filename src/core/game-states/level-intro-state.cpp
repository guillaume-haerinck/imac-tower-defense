#include "level-intro-state.hpp"

#include <SDL2/SDL.h>

#include "core/game.hpp"
#include "core/constants.hpp"

LevelIntroState::LevelIntroState(Game& game) : IGameState(game), m_levelIntro(game.emitter, game.progression)
{
	m_xaml = m_levelIntro;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

LevelIntroState::~LevelIntroState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelIntroState::enter() {
	connectInputs();
}

void LevelIntroState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

	// Render
	m_ui->GetRenderer()->Render();
}

void LevelIntroState::exit() {
	disconnectInputs();
}

void LevelIntroState::onLeftClickUp(const evnt::LeftClickUp& event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void LevelIntroState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void LevelIntroState::onMouseMove(const evnt::MouseMove& event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);
	m_game.emitter.mousePos = event.mousePos;
}
