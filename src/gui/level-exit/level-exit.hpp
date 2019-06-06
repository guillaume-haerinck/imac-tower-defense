#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"
#include "level-exit-bindings.hpp"

class LevelExit : public Noesis::Grid {
public:
	LevelExit(EventEmitter& emitter, Progression& progression);

private:
	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onNextLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

private:
	NS_DECLARE_REFLECTION(LevelExit, Grid);

	Noesis::Ptr<LevelExitBindings> m_bindings;
	EventEmitter& m_emitter;
	Progression& m_progression;
};
