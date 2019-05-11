#include "floating-menu-bindings.hpp"

#include <spdlog/spdlog.h>

#include "events/change-game-state.hpp"
#include "events/progression-updated.hpp"
#include "events/selected.hpp"
#include "core/tags.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "components/transform.hpp"

FloatingMenuBindings::FloatingMenuBindings(EventEmitter& emitter, Progression& progression, entt::DefaultRegistry& registry)
	: m_emitter(emitter), m_progression(progression), m_registry(registry), bHide(true), m_posX(0.0f), m_posY(0.0f)
{
	/*
	std::string text = std::to_string(this->m_progression.getMoney());
	this->SetOutput(text.c_str());

	m_emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string text = std::to_string(this->m_progression.getMoney());
		this->SetOutput(text.c_str());
		});
	*/

	// TODO handle unselection
	m_emitter.on<evnt::Selected>([this](const evnt::Selected & event, EventEmitter & emitter) {
		if (m_registry.has<entityTag::Tower>(event.entity)) {
			const glm::vec2 position = m_registry.get<cmpt::Transform>(event.entity).position;
			this->bHide = false;

			const float posXWindow = imaths::rangeMapping(position.x, 0.0f, PROJ_WIDTH_RAT, 0.0f, WIN_WIDTH);
			const float posYWindow = imaths::rangeMapping(position.y, 0.0f, PROJ_HEIGHT, 0.0f, WIN_HEIGHT);

			this->setPosX(posXWindow);
			this->setPosY(posYWindow);
		}
	});
}

void FloatingMenuBindings::setPosX(float pos) {
	if (m_posX != pos) {
		m_posX = pos;
		OnPropertyChanged("PosX");
	}
}

float FloatingMenuBindings::getPosX() const {
	return m_posX;
}

void FloatingMenuBindings::setPosY(float pos) {
	if (m_posY != pos) {
		m_posY = pos;
		OnPropertyChanged("PosY");
	}
}

float FloatingMenuBindings::getPosY() const {
	return m_posY;
}

/*
const char* FloatingMenuBindings::GetOutput() const {
	return m_output;
}

void FloatingMenuBindings::SetOutput(const char* value) {
	if (!Noesis::String::Equals(m_output, value)) {
		Noesis::String::Copy(m_output, sizeof(m_output), value);
		OnPropertyChanged("Output");
	}
}
*/

