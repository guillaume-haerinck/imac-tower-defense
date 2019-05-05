#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>

#include "events/handlers/event-emitter.hpp"

class GameOver : public Noesis::StackPanel {
public:
	GameOver(EventEmitter& emitter);

private:
	// Events
	void OnButton1Click(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();
	NS_IMPLEMENT_INLINE_REFLECTION(GameOver, StackPanel) {
		NsMeta<Noesis::TypeId>("GameOver");
	}

private:
	EventEmitter& m_emitter;
};