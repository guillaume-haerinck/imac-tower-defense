#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"

class LevelExit : public Noesis::Grid {
public:
	LevelExit(EventEmitter& emitter, Progression& progression);

private:
	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();

private:
	NS_DECLARE_REFLECTION(LevelExit, Grid);

	EventEmitter& m_emitter;
	Progression& m_progression;
};
