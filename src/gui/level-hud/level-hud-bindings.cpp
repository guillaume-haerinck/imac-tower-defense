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
	NsProp("PosX", &LevelHudBindings::getPosX, &LevelHudBindings::setPosX);
	NsProp("PosY", &LevelHudBindings::getPosY, &LevelHudBindings::setPosY);
}

LevelHudBindings::LevelHudBindings() : bHide(true), m_posX(0.0f), m_posY(0.0f)
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

void LevelHudBindings::setPosX(float pos) {
	if (m_posX != pos) {
		m_posX = pos;
		OnPropertyChanged("PosX");
	}
}

float LevelHudBindings::getPosX() const {
	return m_posX;
}

void LevelHudBindings::setPosY(float pos) {
	if (m_posY != pos) {
		m_posY = pos;
		OnPropertyChanged("PosY");
	}
}

float LevelHudBindings::getPosY() const {
	return m_posY;
}

/*
const char* LevelHudBindings::GetOutput() const {
	return m_output;
}

void LevelHudBindings::SetOutput(const char* value) {
	if (!Noesis::String::Equals(m_output, value)) {
		Noesis::String::Copy(m_output, sizeof(m_output), value);
		OnPropertyChanged("Output");
	}
}
*/

