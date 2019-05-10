#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <entt/entt.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"

class LevelHud : public Noesis::StackPanel {
public:
	LevelHud(EventEmitter& emitter, Progression& progression, entt::DefaultRegistry& registry);

private:
	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	void OnMouseEnter(const Noesis::MouseEventArgs& e) override;
	void OnMouseLeave(const Noesis::MouseEventArgs& e) override;

private:
	NS_IMPLEMENT_INLINE_REFLECTION(LevelHud, StackPanel) {
		NsMeta<Noesis::TypeId>("LevelHud");
	}

	EventEmitter& m_emitter;
	Progression& m_progression;
	entt::DefaultRegistry& m_registry;
};

