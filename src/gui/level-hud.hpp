#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"

class LevelHud : public Noesis::StackPanel {
public:
	LevelHud(EventEmitter& emitter, Progression& progression);

private:
	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

private:
	NS_IMPLEMENT_INLINE_REFLECTION(LevelHud, StackPanel) {
		NsMeta<Noesis::TypeId>("LevelHud");
	}

	EventEmitter& m_emitter;
	Progression& m_progression;
};

class ViewModel : public NoesisApp::NotifyPropertyChangedBase {
public:
	ViewModel(EventEmitter& emitter, Progression& progression);
	const char* GetOutput() const;
	void SetOutput(const char* value);

private:
	char m_output[256] = "";
	EventEmitter& m_emitter;
	Progression& m_progression;

	NS_IMPLEMENT_INLINE_REFLECTION(ViewModel, NotifyPropertyChangedBase) {
		NsMeta<Noesis::TypeId>("ViewModel");
		NsProp("Output", &ViewModel::GetOutput, &ViewModel::SetOutput);
	}
};
