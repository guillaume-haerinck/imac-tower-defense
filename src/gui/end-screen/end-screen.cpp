#include "end-screen.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(EndScreen) {
	NsMeta<Noesis::TypeId>("EndScreen");
}

EndScreen::EndScreen(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void EndScreen::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "end-screen.xaml");
}

bool EndScreen::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}
