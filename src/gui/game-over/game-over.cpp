#include "game-over.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(GameOver) {
	NsMeta<Noesis::TypeId>("GameOver");
}

GameOver::GameOver(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void GameOver::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "game-over.xaml");
}

bool GameOver::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}
