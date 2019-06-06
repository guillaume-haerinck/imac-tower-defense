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

LevelIntroBindings::LevelIntroBindings() : m_title("-"), m_imagePath(""), m_text("-") {
}

const char* LevelIntroBindings::getTitle() const {
	return m_title.c_str();
}

void LevelIntroBindings::setTitle(const char* value) {
	if (m_title != value) {
		m_title = value;
		OnPropertyChanged("Title");
	}
}

const char* LevelIntroBindings::getImagePath() const {
	return m_imagePath.c_str();
}

void LevelIntroBindings::setImagePath(const char* value) {
	if (m_imagePath != value) {
		m_imagePath = value;
		OnPropertyChanged("ImagePath");
	}
}

const char* LevelIntroBindings::getText() const {
	return m_text.c_str();
}

void LevelIntroBindings::setText(const char* value) {
	if (m_text != value) {
		m_text = value;
		OnPropertyChanged("Text");
	}
}
