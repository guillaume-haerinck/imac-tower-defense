#include "level-hud.hpp"

#include <NsGui/Button.h>
#include <string>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/game-states/i-game-state.hpp"
#include "events/interactions/construct-selection.hpp"

NS_IMPLEMENT_REFLECTION(LevelHud) {
	NsMeta<Noesis::TypeId>("LevelHud");
}

LevelHud::LevelHud(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression)
{
	m_bindings = *new LevelHudBindings();
	Initialized() += MakeDelegate(this, &LevelHud::OnInitialized);
	InitializeComponent();
}

void LevelHud::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-hud.xaml");
}

bool LevelHud::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onSelectTower);
	NS_CONNECT_EVENT(Noesis::Button, Click, onSelectMirror);
	return false;
}

void LevelHud::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
	m_bindings->setPosX(500.0f);
}

void LevelHud::OnMouseEnter(const Noesis::MouseEventArgs& e) {
	spdlog::info("Mouse entered Noesis !");
	m_emitter.focus = FocusMode::HUD;
}

void LevelHud::OnMouseLeave(const Noesis::MouseEventArgs& e) {
	spdlog::info("Mouse left Noesis !");
	m_emitter.focus = FocusMode::GAME;
}

void LevelHud::OnMouseDown(const Noesis::MouseButtonEventArgs& e) {
	spdlog::info("Mouse click noesis !");
}

void LevelHud::onSelectTower(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ConstructSelection>(ConstructibleType::TOWER_BASIC);
}

void LevelHud::onSelectMirror(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ConstructSelection>(ConstructibleType::MIRROR_BASIC);
}