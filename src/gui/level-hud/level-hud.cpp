#include "level-hud.hpp"

#include <NsGui/Button.h>
#include <string>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/constants.hpp"
#include "core/game-states/i-game-state.hpp"
#include "events/interactions/construct-selection.hpp"
#include "events/interactions/delete-entity.hpp"
#include "events/wave-updated.hpp"

NS_IMPLEMENT_REFLECTION(LevelHud) {
	NsMeta<Noesis::TypeId>("LevelHud");
}

LevelHud::LevelHud(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression)
{
	m_bindings = *new LevelHudBindings();
	Initialized() += MakeDelegate(this, &LevelHud::OnInitialized);
	InitializeComponent();

	m_emitter.on<evnt::WaveUpdated>([this](const evnt::WaveUpdated & event, EventEmitter & emitter) {
		switch (event.state) {
		case WaveState::PENDING:
			this->m_bindings->setTimer("Get ready...");
			break;

		case WaveState::DURING:
			this->m_bindings->setTimer(std::to_string(event.timer).c_str());
			break;

		case WaveState::NO:
			this->m_bindings->setTimer("Kill what remains");
			break;

		default:
			break;
		}
	});
}

void LevelHud::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-hud.xaml");
}

bool LevelHud::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onSelectTowerSlow);
	NS_CONNECT_EVENT(Noesis::Button, Click, onSelectMirror);
	NS_CONNECT_EVENT(Noesis::Button, Click, onDeleteEntity);
	return false;
}

void LevelHud::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

void LevelHud::OnMouseEnter(const Noesis::MouseEventArgs& e) {
	m_emitter.focus = FocusMode::HUD;
}

void LevelHud::OnMouseLeave(const Noesis::MouseEventArgs& e) {
	m_emitter.focus = FocusMode::GAME;
}

void LevelHud::OnMouseDown(const Noesis::MouseButtonEventArgs& e) {
}

void LevelHud::onSelectTowerSlow(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ConstructSelection>(ConstructibleType::TOWER_SLOW);
}

void LevelHud::onSelectMirror(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ConstructSelection>(ConstructibleType::MIRROR_BASIC);
}

void LevelHud::onDeleteEntity(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::DeleteEntity>(m_lastSelectedEntity);
}

/* ------------------------ SETTERS ----------------------- */

void LevelHud::setOptionsPosition(glm::vec2 pos) {
	// Invert pos Y to match 0.0 on the top
	pos.y = WIN_HEIGHT - pos.y;

	// Make the pivot point bottom left with an offset corresponding to menu size
	pos.y -= 55;
	pos.x += 25;

	setOptionsVisibilityTo(true);
	m_bindings->setOptionsPosX(pos.x);
	m_bindings->setOptionsPosY(pos.y);
}

void LevelHud::setOptionsVisibilityTo(bool show) {
	if (show) {
		m_bindings->setOptionsVisibility("Visible");
	} else {
		m_bindings->setOptionsVisibility("Collapsed");
	}
}

void LevelHud::setSelectedEntity(std::uint32_t id) {
	m_lastSelectedEntity = id;
}
