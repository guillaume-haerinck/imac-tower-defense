#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>

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
	NsString m_optionsVisibility;
	NsString m_startWaveBtnVisibility;
	float m_startWaveBtnPosY;
	NsString m_timer;
	NsString m_life;
	NsString m_mirrorNumber;
	NsString m_slowNumber;
};