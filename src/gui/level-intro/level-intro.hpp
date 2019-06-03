#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"

class LevelIntro : public Noesis::Grid {
public:
	LevelIntro(EventEmitter& emitter);

private:
	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();

private:
	NS_DECLARE_REFLECTION(LevelIntro, Grid);
	EventEmitter& m_emitter;
};
