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
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartGame);
	NS_CONNECT_EVENT(Noesis::Button, Click, onQuitGame);
    return false;
}

void TitleScreen::onStartGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, 1);
}

void TitleScreen::onQuitGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {

}