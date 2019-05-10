#include "level-hud.hpp"

#include <NsGui/Button.h>
#include <string>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "level-hud-bindings/floating-menu-bindings.hpp"
#include "core/game-states/i-game-state.hpp"


LevelHud::LevelHud(EventEmitter& emitter, Progression& progression, entt::DefaultRegistry& registry)
	: m_emitter(emitter), m_progression(progression), m_registry(registry)
{
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
	SetDataContext(Noesis::MakePtr<FloatingMenuBindings>(m_emitter, m_progression, m_registry));
}

void LevelHud::OnMouseEnter(const Noesis::MouseEventArgs& e) {
	spdlog::info("Mouse entered Noesis !");
	m_emitter.focus = FocusMode::HUD;
}

void LevelHud::OnMouseLeave(const Noesis::MouseEventArgs& e) {
	spdlog::info("Mouse left Noesis !");
	m_emitter.focus = FocusMode::GAME;
}
