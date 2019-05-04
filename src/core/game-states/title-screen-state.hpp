#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>

#include "events/handlers/event-emitter.hpp"
#include "gui/title-screen.hpp"

class TitleScreenState {
public:
	TitleScreenState(EventEmitter& emitter);
	~TitleScreenState();

	void update();

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	TitleScreen m_titleScreen;
	EventEmitter& m_emitter;
};
