#include "level-hud.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

LevelHud::LevelHud(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	InitializeComponent();
}

void LevelHud::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-hud.xaml");
}

bool LevelHud::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}

