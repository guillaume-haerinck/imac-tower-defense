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

void LevelState::onEnter() {
	// Set event subscription for used systems 
	m_game.animationSystem->connectEvents();
	m_game.attackSystem->connectEvents();
	m_game.constructionSystem->connectEvents();
	m_game.healthSystem->connectEvents();
	m_game.movementSystem->connectEvents();
	m_game.renderSystem->connectEvents();
	m_game.waveSystem->connectEvents();

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

void LevelState::update(float deltatime) {
	// Noesis gui update
	m_ui->Update(SDL_GetTicks());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT));
	GLCall(glClearStencil(0));

	// TODO create deltatima with this 6/1000 factor ? must check where it is used and why there is such a unit
	// TODO ne pas utiliser le deltatime comme ça, il est quasiment fixe à chaque frame
	m_game.animationSystem->update(deltatime / 1000 * 6); 
	m_game.movementSystem->update(deltatime);
	m_game.renderSystem->update(deltatime);
	m_game.attackSystem->update(deltatime);
	m_ui->GetRenderer()->Render();
}

void LevelState::onExit() {
	// Remove event listenner
	m_game.emitter.erase(*m_clickUpCon);
	m_game.emitter.erase(*m_clickDownCon);
	m_clickUpCon.reset();
	m_clickDownCon.reset();
}
