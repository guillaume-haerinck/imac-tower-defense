#include "title-screen.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

/* TODO Data binding
    https://www.noesisengine.com/docs/Gui.Core.DataBindingTutorial.html
    https://www.noesisengine.com/docs/Gui.Core.UserControlTutorial.html
    https://www.noesisengine.com/docs/Gui.Core.CustomControlTutorial.html
*/

TitleScreen::TitleScreen(EventEmitter& emitter) : m_emitter(emitter) {
    InitializeComponent();
}

void TitleScreen::InitializeComponent() {
    Noesis::GUI::LoadComponent(this, "title-screen.xaml");
}

bool TitleScreen::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
    NS_CONNECT_EVENT(Noesis::Button, Click, OnButton1Click);
    NS_CONNECT_EVENT(Noesis::Button, Click, OnButton2Click);
    return false;
}

void TitleScreen::OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
    printf("Title screen : Button1 was clicked\n");
	m_emitter.publish<evnt::ChangeGameState>(GameState::LEVEL, 1);
}

void TitleScreen::OnButton2Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
    printf("Title screen : Button2 was clicked\n");
}
