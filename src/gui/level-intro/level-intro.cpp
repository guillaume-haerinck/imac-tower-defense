#include "level-intro.hpp"

#include <NsGui/Button.h>

#include "events/change-game-state.hpp"
#include "events/progression-updated.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(LevelIntro) {
	NsMeta<Noesis::TypeId>("LevelIntro");
}

LevelIntro::LevelIntro(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	m_bindings = *new LevelIntroBindings();
	Initialized() += MakeDelegate(this, &LevelIntro::OnInitialized);
	InitializeComponent();

	m_emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {

	});
}

void LevelIntro::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-intro.xaml");
}

bool LevelIntro::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartLevel);
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	return false;
}

void LevelIntro::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

void LevelIntro::onStartLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::LEVEL, 1);
}

void LevelIntro::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::TITLE_SCREEN, 1);
}
