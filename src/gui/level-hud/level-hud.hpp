#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"
#include "level-hud-bindings.hpp"

class LevelHud : public Noesis::StackPanel {
public:
	LevelHud(EventEmitter& emitter, Progression& progression);

private:
	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onSelectTower(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onSelectMirror(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

	// Input events
	void OnMouseEnter(const Noesis::MouseEventArgs& e) override;
	void OnMouseLeave(const Noesis::MouseEventArgs& e) override;
	void OnMouseDown(const Noesis::MouseButtonEventArgs& e) override;

private:
	NS_DECLARE_REFLECTION(LevelHud, StackPanel)

	Noesis::Ptr<LevelHudBindings> m_bindings;
	EventEmitter& m_emitter;
	Progression& m_progression;
};
