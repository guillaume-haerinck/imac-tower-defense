#include "level-hud-bindings.hpp"

#include <spdlog/spdlog.h>

#include "events/change-game-state.hpp"
#include "events/progression-updated.hpp"
#include "events/selected.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/transform.hpp"

NS_IMPLEMENT_REFLECTION(LevelHudBindings) {
	NsMeta<Noesis::TypeId>("LevelHudBindings");
	NsProp("OptionsPosX", &LevelHudBindings::getOptionsPosX, &LevelHudBindings::setOptionsPosX);
	NsProp("OptionsPosY", &LevelHudBindings::getOptionsPosY, &LevelHudBindings::setOptionsPosY);
	NsProp("OptionsVisibility", &LevelHudBindings::getOptionsVisibility, &LevelHudBindings::setOptionsVisibility);
	NsProp("StartWaveBtnVisibility", &LevelHudBindings::getStartWaveBtnVisibility, &LevelHudBindings::setStartWaveBtnVisibility);
	NsProp("StartWaveBtnPosY", &LevelHudBindings::getStartWaveBtnPosY, &LevelHudBindings::setStartWaveBtnPosY);
	NsProp("Timer", &LevelHudBindings::getTimer, &LevelHudBindings::setTimer);
	NsProp("Life", &LevelHudBindings::getLife, &LevelHudBindings::setLife);
	NsProp("MirrorNumber", &LevelHudBindings::getMirrorNumber, &LevelHudBindings::setMirrorNumber);
	NsProp("SlowNumber", &LevelHudBindings::getSlowNumber, &LevelHudBindings::setSlowNumber);
}

LevelHudBindings::LevelHudBindings()
: m_optionsVisibility("Visible"), m_optionsPosX(0.0f), m_optionsPosY(-500.0f),
  m_startWaveBtnVisibility("Visible"), m_startWaveBtnPosY(0.0f),
  m_timer("-"), m_life("100 / 100"), m_mirrorNumber("-"), m_slowNumber("-")
{
	/*
	std::string text = std::to_string(this->m_progression.getMoney());
	this->SetOutput(text.c_str());

	m_emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string text = std::to_string(this->m_progression.getMoney());
		this->SetOutput(text.c_str());
		});
	*/
}

/* ---------------- Flying option menu -------------- */

void LevelHudBindings::setOptionsPosX(float pos) {
	if (m_optionsPosX != pos) {
		m_optionsPosX = pos;
		OnPropertyChanged("OptionsPosX");
	}
}

float LevelHudBindings::getOptionsPosX() const {
	return m_optionsPosX;
}

void LevelHudBindings::setOptionsPosY(float pos) {
	if (m_optionsPosY != pos) {
		m_optionsPosY = pos;
		OnPropertyChanged("OptionsPosY");
	}
}

float LevelHudBindings::getOptionsPosY() const {
	return m_optionsPosY;
}


const char* LevelHudBindings::getOptionsVisibility() const {
	return m_optionsVisibility;
}

void LevelHudBindings::setOptionsVisibility(const char* value) {
	if (!Noesis::String::Equals(m_optionsVisibility, value)) {
		Noesis::String::Copy(m_optionsVisibility, sizeof(m_optionsVisibility), value);
		//OnPropertyChanged("Visibitity");

		// Temp fix because does not work at runtime
		if (Noesis::String::Equals("Collapsed", value)) {
			m_optionsPosY = -500;
			OnPropertyChanged("OptionsPosY");
		}
	}
}

/* ------------------- TopBar ---------------------- */
const char* LevelHudBindings::getStartWaveBtnVisibility() const {
	return m_startWaveBtnVisibility;
}

void LevelHudBindings::setStartWaveBtnVisibility(const char* value) {
	if (!Noesis::String::Equals(m_startWaveBtnVisibility, value)) {
		Noesis::String::Copy(m_startWaveBtnVisibility, sizeof(m_startWaveBtnVisibility), value);
		//OnPropertyChanged("Visibitity");

		// Temp fix because does not work at runtime
		if (Noesis::String::Equals("Collapsed", value)) {
			m_startWaveBtnPosY = 500;
			OnPropertyChanged("StartWaveBtnVisibility");
		}
	}
}

void LevelHudBindings::setStartWaveBtnPosY(float pos) {
	if (m_startWaveBtnPosY != pos) {
		m_startWaveBtnPosY = pos;
		OnPropertyChanged("StartWaveBtnPosY");
	}
}

float LevelHudBindings::getStartWaveBtnPosY() const {
	return m_startWaveBtnPosY;
}

const char* LevelHudBindings::getTimer() const {
	return m_timer;
}

void LevelHudBindings::setTimer(const char* value) {
	if (!Noesis::String::Equals(m_timer, value)) {
		Noesis::String::Copy(m_timer, sizeof(m_timer), value);
		OnPropertyChanged("Timer");
	}
}

const char* LevelHudBindings::getLife() const {
	return m_life;
}

void LevelHudBindings::setLife(const char* value) {
	if (!Noesis::String::Equals(m_life, value)) {
		Noesis::String::Copy(m_life, sizeof(m_life), value);
		OnPropertyChanged("Life");
	}
}

/* ------------ BottomBar ------------------ */

const char* LevelHudBindings::getMirrorNumber() const {
	return m_mirrorNumber;
}

void LevelHudBindings::setMirrorNumber(const char* value) {
	if (!Noesis::String::Equals(m_mirrorNumber, value)) {
		Noesis::String::Copy(m_mirrorNumber, sizeof(m_mirrorNumber), value);
		OnPropertyChanged("MirrorNumber");
	}
}

const char* LevelHudBindings::getSlowNumber() const {
	return m_slowNumber;
}

void LevelHudBindings::setSlowNumber(const char* value) {
	if (!Noesis::String::Equals(m_slowNumber, value)) {
		Noesis::String::Copy(m_slowNumber, sizeof(m_slowNumber), value);
		OnPropertyChanged("SlowNumber");
	}
}

