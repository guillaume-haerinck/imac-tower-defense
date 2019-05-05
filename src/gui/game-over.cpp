#include "game-over.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

GameOver::GameOver(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void GameOver::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "game-over.xaml");
}

bool GameOver::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, OnButton1Click);
	return false;
}

void GameOver::OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	printf("Game over screen : Button1 was clicked\n");
	m_emitter.publish<evnt::ChangeGameState>(GameState::LEVEL, 1);
}
