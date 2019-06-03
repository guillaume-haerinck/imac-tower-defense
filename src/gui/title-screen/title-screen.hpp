#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"

class TitleScreen: public Noesis::Grid {
public:
    TitleScreen(EventEmitter& emitter);

private:
    // Events
    bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onStartGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onQuitGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

    // Init 
    void InitializeComponent();
	
private:
	NS_DECLARE_REFLECTION(TitleScreen, Grid);
	EventEmitter& m_emitter;
};
