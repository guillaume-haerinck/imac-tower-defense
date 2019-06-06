#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"

class GameOver : public Noesis::Grid {
public:
	GameOver(EventEmitter& emitter, Progression& progression);

private:
	// Init 
	void InitializeComponent();

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onRestart(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

private:
	NS_DECLARE_REFLECTION(GameOver, Grid);
	EventEmitter& m_emitter;
	Progression& m_progression;
};
