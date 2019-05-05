#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"

class LevelHud : public Noesis::StackPanel {
public:
	LevelHud(EventEmitter& emitter, Progression& progression);

private:
	// Events
	void OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();
	NS_IMPLEMENT_INLINE_REFLECTION(LevelHud, StackPanel) {
		NsMeta<Noesis::TypeId>("LevelHud");
	}

private:
	EventEmitter& m_emitter;
	Progression& m_progression;
};
