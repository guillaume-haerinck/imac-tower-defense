#include "level-hud.hpp"

#include <NsGui/Button.h>
#include "events/change-game-state.hpp"
#include "core/game-states/i-game-state.hpp"

LevelHud::LevelHud(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	Initialized() += MakeDelegate(this, &LevelHud::OnInitialized);
	InitializeComponent();
}

void LevelHud::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-hud.xaml");
}

bool LevelHud::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}

void LevelHud::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(Noesis::MakePtr<ViewModel>());
}

ViewModel::ViewModel() : m_output("wooooorks") {}

const char* ViewModel::GetOutput() const {
	return m_output;
}

void ViewModel::SetOutput(const char* value) {
	if (!Noesis::String::Equals(m_output, value)) {
		Noesis::String::Copy(m_output, sizeof(m_output), value);
		OnPropertyChanged("Output");
	}
}

