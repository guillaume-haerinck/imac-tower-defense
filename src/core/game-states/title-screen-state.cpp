#include "title-screen-state.hpp"

#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <NsGui/Grid.h>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "gui/title-screen.hpp"

TitleScreenState::TitleScreenState() {
	TitleScreen titleScreen;
	Noesis::Ptr<Noesis::FrameworkElement> xaml = titleScreen;
	m_ui = Noesis::GUI::CreateView(xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	m_ui->SetSize(WIN_WIDTH, WIN_HEIGHT);

	// TODO listen to .. via event emitter
	// m_ui->MouseButtonUp(e.button.x, e.button.y, Noesis::MouseButton_Left);
	// m_ui->MouseButtonDown(e.button.x, e.button.y, Noesis::MouseButton_Left);
}

TitleScreenState::~TitleScreenState() {
	m_ui->GetRenderer()->Shutdown();
}

void TitleScreenState::update() {
	// Noesis gui update
	// FIXME null pointer exeption thrown
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