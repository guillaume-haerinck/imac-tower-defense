#include "game-over-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "core/game.hpp"

GameOverState::GameOverState(Game& game) : IGameState(game), m_gameOver(m_game.emitter)
{
	m_xaml = m_gameOver;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

GameOverState::~GameOverState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void GameOverState::onEnter() {
	// Remove event subscriptions to unused systems
	m_game.animationSystem->disconnectInputs();
	m_game.attackSystem->disconnectInputs();
	m_game.constructionSystem->disconnectInputs();
	m_game.healthSystem->disconnectInputs();
	m_game.movementSystem->disconnectInputs();
	m_game.renderSystem->disconnectInputs();
	m_game.waveSystem->disconnectInputs();

	// Listen to event and copy connection object
	auto connectionDown = m_game.emitter.on<evnt::LeftClickDown>([this](const evnt::LeftClickDown & event, EventEmitter & emitter) {
		this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
	});
	m_clickDownCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>>(connectionDown);

	auto connectionUp = m_game.emitter.on<evnt::LeftClickUp>([this](const evnt::LeftClickUp & event, EventEmitter & emitter) {
		this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
	});
	m_clickUpCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>>(connectionUp);
}

void GameOverState::update(float deltatime) {
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

void GameOverState::onExit() {
	// Remove event listenner
	m_game.emitter.erase(*m_clickUpCon);
	m_game.emitter.erase(*m_clickDownCon);
	m_clickUpCon.reset();
	m_clickDownCon.reset();
}
