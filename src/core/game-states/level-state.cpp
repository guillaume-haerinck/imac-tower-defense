#include "level-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"

LevelState::LevelState(Progression& progression,
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
	m_levelHud(emitter, progression)
{
	m_xaml = m_levelHud;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);
}

LevelState::~LevelState() {
	m_ui->GetRenderer()->Shutdown();
}

void LevelState::onEnter() {
	// Set event subscription for used systems 
	m_animationSystem.connectEvents();
	m_attackSystem.connectEvents();
	m_constructionSystem.connectEvents();
	m_healthSystem.connectEvents();
	m_movementSystem.connectEvents();
	m_renderSystem.connectEvents();
	m_waveSystem.connectEvents();

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
	m_animationSystem.update(deltatime / 1000 * 6); 
	m_movementSystem.update(deltatime);
	m_attackSystem.update(deltatime);
	m_renderSystem.update(deltatime);
	m_ui->GetRenderer()->Render();
}

void LevelState::onExit() {
	// Remove event listenner
	m_emitter.erase(*m_clickUpCon);
	m_emitter.erase(*m_clickDownCon);
	m_clickUpCon.reset();
	m_clickDownCon.reset();
}
