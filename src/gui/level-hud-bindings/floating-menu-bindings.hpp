#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <entt/entt.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"


class FloatingMenuBindings : public NoesisApp::NotifyPropertyChangedBase {
public:
	FloatingMenuBindings(EventEmitter& emitter, Progression& progression, entt::DefaultRegistry& registry);

	//const char* GetOutput() const;
	//void SetOutput(const char* value);

	void setPosX(float pos);
	float getPosX() const;

	void setPosY(float pos);
	float getPosY() const;

private:
	float m_posX;
	float m_posY;
	bool bHide;

	EventEmitter& m_emitter;
	Progression& m_progression;
	entt::DefaultRegistry& m_registry;

	NS_IMPLEMENT_INLINE_REFLECTION(FloatingMenuBindings, NotifyPropertyChangedBase) {
		NsMeta<Noesis::TypeId>("FloatingMenuBindings");
		NsProp("PosX", &FloatingMenuBindings::getPosX, &FloatingMenuBindings::setPosX);
		NsProp("PosY", &FloatingMenuBindings::getPosY, &FloatingMenuBindings::setPosY);
	}
};