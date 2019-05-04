#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>

#include "gui/title-screen.hpp"

class TitleScreenState {
public:
	TitleScreenState();
	~TitleScreenState();

	void update();

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	TitleScreen m_titleScreen;
};
