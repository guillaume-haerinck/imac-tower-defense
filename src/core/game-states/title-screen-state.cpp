#include "title-screen-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"

TitleScreenState::TitleScreenState(Progression& progression,
	EventEmitter& emitter,
	AnimationSystem& animationSystem,
	AttackSystem& attackSystem,
	ConstructionSystem& constructionSystem,
	HealthSystem& healthSystem,
	MovementSystem& movementSystem,
	RenderSystem& renderSystem,
	WaveSystem& waveSystem)
: IGameState(progression,
		emitter,
		animationSystem,
		attackSystem,
		constructionSystem,
		healthSystem,
		movementSystem,
		renderSystem,
		waveSystem),
	m_titleScreen(emitter)
{
	m_xaml = m_titleScreen;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

TitleScreenState::~TitleScreenState() {
	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void TitleScreenState::onEnter() {
	// Remove subscription for unsused systems
	m_animationSystem.disconnectEvents();
	m_attackSystem.disconnectEvents();
	m_constructionSystem.disconnectEvents();
	m_healthSystem.disconnectEvents();
	m_movementSystem.disconnectEvents();
	m_renderSystem.disconnectEvents();
	m_waveSystem.disconnectEvents();

	// Listen to event and copy connection object
	auto connectionDown = m_emitter.on<evnt::LeftClickDown>([this](const evnt::LeftClickDown & event, EventEmitter & emitter) {
		this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
	});
	m_clickDownCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>>(connectionDown);

	auto connectionUp = m_emitter.on<evnt::LeftClickUp>([this](const evnt::LeftClickUp & event, EventEmitter & emitter) {
		this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
	});
	m_clickUpCon = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>>(connectionUp);
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

void TitleScreenState::onExit() {
	// Remove event listenner
	m_emitter.erase(*m_clickUpCon);
	m_emitter.erase(*m_clickDownCon);
	m_clickUpCon.reset();
	m_clickDownCon.reset();
}
