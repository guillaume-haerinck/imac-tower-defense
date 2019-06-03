#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"
#include "level-intro-bindings.hpp"

class LevelIntro : public Noesis::Grid {
public:
	LevelIntro(EventEmitter& emitter, Progression& progression);

private:
	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onStartLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

private:
	NS_DECLARE_REFLECTION(LevelIntro, Grid);

	Noesis::Ptr<LevelIntroBindings> m_bindings;
	EventEmitter& m_emitter;
	Progression& m_progression;
};
