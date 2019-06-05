#include "level-exit-bindings.hpp"

#include <spdlog/spdlog.h>

#include "events/change-game-state.hpp"
#include "events/progression-updated.hpp"
#include "events/selected.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/transform.hpp"

NS_IMPLEMENT_REFLECTION(LevelExitBindings) {
	NsMeta<Noesis::TypeId>("LevelExitBindings");
	NsProp("Text", &LevelExitBindings::getText, &LevelExitBindings::setText);
}

LevelExitBindings::LevelExitBindings() : m_text("Good luck !") {
}

const char* LevelExitBindings::getText() const {
	return m_text.c_str();
}

void LevelExitBindings::setText(const char* value) {
	if (m_text != value) {
		m_text = value;
		OnPropertyChanged("Text");
	}
}
