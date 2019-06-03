#include "level-intro-bindings.hpp"

#include <spdlog/spdlog.h>

#include "events/change-game-state.hpp"
#include "events/progression-updated.hpp"
#include "events/selected.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/transform.hpp"

NS_IMPLEMENT_REFLECTION(LevelIntroBindings) {
	NsMeta<Noesis::TypeId>("LevelIntroBindings");
	NsProp("Title", &LevelIntroBindings::getTitle, &LevelIntroBindings::setTitle);
	NsProp("ImagePath", &LevelIntroBindings::getImagePath, &LevelIntroBindings::setImagePath);
	NsProp("Text", &LevelIntroBindings::getText, &LevelIntroBindings::setText);
}

LevelIntroBindings::LevelIntroBindings() : m_title("LEVEL I"), m_imagePath(""), m_text("Good luck !") {
	/*
	std::string text = std::to_string(this->m_progression.getMoney());
	this->SetOutput(text.c_str());

	m_emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string text = std::to_string(this->m_progression.getMoney());
		this->SetOutput(text.c_str());
		});
	*/
}

const char* LevelIntroBindings::getTitle() const {
	return m_title;
}

void LevelIntroBindings::setTitle(const char* value) {
	if (!Noesis::String::Equals(m_title, value)) {
		Noesis::String::Copy(m_title, sizeof(m_title), value);
		OnPropertyChanged("Title");
	}
}

const char* LevelIntroBindings::getImagePath() const {
	return m_imagePath;
}

void LevelIntroBindings::setImagePath(const char* value) {
	if (!Noesis::String::Equals(m_imagePath, value)) {
		Noesis::String::Copy(m_imagePath, sizeof(m_imagePath), value);
		OnPropertyChanged("ImagePath");
	}
}

const char* LevelIntroBindings::getText() const {
	return m_text;
}

void LevelIntroBindings::setText(const char* value) {
	if (!Noesis::String::Equals(m_text, value)) {
		Noesis::String::Copy(m_text, sizeof(m_text), value);
		OnPropertyChanged("Text");
	}
}
