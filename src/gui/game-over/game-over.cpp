#include "game-over.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(GameOver) {
	NsMeta<Noesis::TypeId>("GameOver");
}

GameOver::GameOver(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	InitializeComponent();
}

void GameOver::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "game-over.xaml");
}

bool GameOver::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	NS_CONNECT_EVENT(Noesis::Button, Click, onRestart);
	return false;
}

void GameOver::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::TITLE_SCREEN, 0);
}

void GameOver::onRestart(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, m_progression.getLevelNumber());
}