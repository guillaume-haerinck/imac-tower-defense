#include "title-screen-state.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "events/left-click-down.hpp"
#include "events/left-click-up.hpp"

TitleScreenState::TitleScreenState(EventEmitter& emitter, AnimationSystem& animationSystem, MovementSystem& movementSystem, AttackSystem& attackSystem, RenderSystem& renderSystem)
: IGameState(emitter, animationSystem, movementSystem, attackSystem, renderSystem), m_titleScreen(emitter)
{
	m_xaml = m_titleScreen;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);

	m_emitter.on<evnt::LeftClickDown>([this](const evnt::LeftClickDown & event, EventEmitter & emitter) {
		this->m_ui->MouseButtonDown(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
	});

	m_emitter.on<evnt::LeftClickUp>([this](const evnt::LeftClickUp & event, EventEmitter & emitter) {
		this->m_ui->MouseButtonUp(event.mousePosSdlCoord.x, event.mousePosSdlCoord.y, Noesis::MouseButton_Left);
	});
}

TitleScreenState::~TitleScreenState() {
	m_ui->GetRenderer()->Shutdown();
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

void TitleScreenState::onEnter() {

}

void TitleScreenState::onExit() {

}
