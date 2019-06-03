#include "title-screen.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(TitleScreen) {
	NsMeta<Noesis::TypeId>("TitleScreen");
}

TitleScreen::TitleScreen(EventEmitter& emitter) : m_emitter(emitter) {
    InitializeComponent();
}

void TitleScreen::InitializeComponent() {
    Noesis::GUI::LoadComponent(this, "title-screen.xaml");
}

bool TitleScreen::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
    return false;
}

