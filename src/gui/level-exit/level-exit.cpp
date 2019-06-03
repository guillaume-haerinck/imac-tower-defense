#include "level-exit.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(LevelExit) {
	NsMeta<Noesis::TypeId>("LevelExit");
}

LevelExit::LevelExit(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void LevelExit::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-exit.xaml");
}

bool LevelExit::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}

