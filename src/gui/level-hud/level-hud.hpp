#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"
#include "level-hud-bindings.hpp"

class LevelHud : public Noesis::Grid {
public:
	LevelHud(EventEmitter& emitter, Progression& progression);

	// Setters
	void setOptionsPosition(glm::vec2 pos);
	void setOptionsVisibilityTo(bool show);

	void setSelectedEntity(std::uint32_t id);

	// Getters

private:
	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onSelectTowerSlow(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onSelectMirror(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onDeleteEntity(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

	// Input events
	void OnMouseEnter(const Noesis::MouseEventArgs& e) override;
	void OnMouseLeave(const Noesis::MouseEventArgs& e) override;
	void OnMouseDown(const Noesis::MouseButtonEventArgs& e) override;

private:
	NS_DECLARE_REFLECTION(LevelHud, Grid)

	Noesis::Ptr<LevelHudBindings> m_bindings;
	EventEmitter& m_emitter;
	Progression& m_progression;
	std::uint32_t m_lastSelectedEntity;
};
