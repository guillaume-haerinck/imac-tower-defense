#pragma once

#include <NsGui/StackPanel.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <entt/entt.hpp>

#include "events/handlers/event-emitter.hpp"
#include "core/progression.hpp"


class LevelHudBindings : public NoesisApp::NotifyPropertyChangedBase {
public:
	LevelHudBindings();

	//const char* GetOutput() const;
	//void SetOutput(const char* value);

	void setPosX(float pos);
	float getPosX() const;

	void setPosY(float pos);
	float getPosY() const;

private:
	NS_DECLARE_REFLECTION(LevelHudBindings, NotifyPropertyChangedBase)

	float m_posX;
	float m_posY;
	bool bHide;
};