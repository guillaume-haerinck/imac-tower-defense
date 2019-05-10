#include "level-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "core/game.hpp"
#include "logger/gl-log-handler.hpp"

LevelState::LevelState(Game& game): IGameState(game), m_levelHud(game.emitter, game.progression)
{
	m_xaml = m_levelHud;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

LevelState::~LevelState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelState::enter() {
	// Set event subscription for used systems 
	m_game.animationSystem->connectInputs();
	m_game.attackSystem->connectInputs();
	m_game.constructionSystem->connectInputs();
	m_game.healthSystem->connectInputs();
	m_game.movementSystem->connectInputs();
	m_game.renderSystem->connectInputs();
	m_game.waveSystem->connectInputs();

	// Subscribe self to inputs
	connectInputs();
}

void LevelState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	restoreGpuState();

	// TODO ne pas utiliser le deltatime comme ça, il est quasiment fixe à chaque frame
	m_game.animationSystem->update(deltatime); 
	m_game.movementSystem->update(deltatime);
	m_game.renderSystem->update(deltatime);
	m_game.attackSystem->update(deltatime);
	m_ui->GetRenderer()->Render();

	switch (checkVictoryConditions()) {
	case GameplayState::LOOSE:
		// change game state to game over screen ? Use a personalized message
		break;

	case GameplayState::WIN:
		// change to next level introduction
		break;

	default:
		break;
	}
}

GameplayState LevelState::checkVictoryConditions() {
	return GameplayState::PLAY;
}

void LevelState::exit() {
	// Remove event listenner
	disconnectInputs();
}


void LevelState::onLeftClickUp(const evnt::LeftClickUp& event) {
	this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void LevelState::onLeftClickDown(const evnt::LeftClickDown& event) {
	this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
}

void LevelState::onMouseMove(const evnt::MouseMove& event) {
	this->m_ui->MouseMove(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y);
}