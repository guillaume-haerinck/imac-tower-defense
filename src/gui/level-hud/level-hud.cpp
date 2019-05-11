#include "level-hud.hpp"

#include <NsGui/Button.h>
#include <string>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(LevelHud) {
	NsMeta<Noesis::TypeId>("LevelHud");
}

LevelHud::LevelHud(EventEmitter& emitter, Progression& progression, entt::DefaultRegistry& registry)
	: m_emitter(emitter), m_progression(progression), m_registry(registry)
{
	m_bindings = *new LevelHudBindings();
	Initialized() += MakeDelegate(this, &LevelHud::OnInitialized);
	InitializeComponent();

	/*
	// TODO handle unselection
	m_emitter.on<evnt::Selected>([this](const evnt::Selected & event, EventEmitter & emitter) {
		if (m_registry.has<entityTag::Tower>(event.entity)) {
			const glm::vec2 position = m_registry.get<cmpt::Transform>(event.entity).position;
			this->bHide = false;

			const float posXWindow = imaths::rangeMapping(position.x, 0.0f, PROJ_WIDTH_RAT, 0.0f, WIN_WIDTH);
			const float posYWindow = imaths::rangeMapping(position.y, 0.0f, PROJ_HEIGHT, 0.0f, WIN_HEIGHT);

			this->setPosX(posXWindow);
			this->setPosY(posYWindow);
		}
	});
	*/
}

void LevelHud::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-hud.xaml");
}

bool LevelHud::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
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
