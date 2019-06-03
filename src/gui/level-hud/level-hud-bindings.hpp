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

	// Flying option menu
	const char* getOptionsVisibility() const;
	void setOptionsVisibility(const char* value);

	void setOptionsPosX(float pos);
	float getOptionsPosX() const;

	void setOptionsPosY(float pos);
	float getOptionsPosY() const;

	// TopBar
	const char* getStartWaveBtnVisibility() const;
	void setStartWaveBtnVisibility(const char* value);

	void setStartWaveBtnPosY(float pos);
	float getStartWaveBtnPosY() const;

	const char* getTimer() const;
	void setTimer(const char* value);

	const char* getLife() const;
	void setLife(const char* value);

	// BottomBar
	const char* getMirrorNumber() const;
	void setMirrorNumber(const char* value);

	const char* getSlowNumber() const;
	void setSlowNumber(const char* value);

private:
	NS_DECLARE_REFLECTION(LevelHudBindings, NotifyPropertyChangedBase)

	float m_optionsPosX;
	float m_optionsPosY;
	char m_optionsVisibility[256];
	char m_startWaveBtnVisibility[256];
	float m_startWaveBtnPosY;
	char m_timer[256];
	char m_life[256];
	char m_mirrorNumber[256];
	char m_slowNumber[256];
};