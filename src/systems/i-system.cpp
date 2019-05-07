#include "i-system.hpp"

void ISystem::connectInputs() {
	if (m_bConnected == false) {
		auto conMouseMove = m_emitter.on<evnt::MouseMove>([this](const evnt::MouseMove & event, EventEmitter & emitter) {
			this->onMouseMove(event);
		});
		m_mouseMove = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::MouseMove>>(conMouseMove);


		auto conLeftClickUp = m_emitter.on<evnt::LeftClickUp>([this](const evnt::LeftClickUp & event, EventEmitter & emitter) {
			this->onLeftClickUp(event);
		});
		m_leftClickUp = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>>(conLeftClickUp);


		auto conLeftClickDown = m_emitter.on<evnt::LeftClickDown>([this](const evnt::LeftClickDown & event, EventEmitter & emitter) {
			this->onLeftClickDown(event);
		});
		m_leftClickDown = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>>(conLeftClickDown);


		auto conRightClickUp = m_emitter.on<evnt::RightClickUp>([this](const evnt::RightClickUp & event, EventEmitter & emitter) {
			this->onRightClickUp(event);
		});
		m_rightClickUp = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>>(conRightClickUp);


		auto conRightClickDown = m_emitter.on<evnt::RightClickDown>([this](const evnt::RightClickDown & event, EventEmitter & emitter) {
			this->onRightClickDown(event);
		});
		m_rightClickDown = std::make_unique<entt::Emitter<EventEmitter>::Connection<evnt::RightClickDown>>(conRightClickDown);

		m_bConnected = true;
	}
}

void ISystem::disconnectInputs() {
	if (m_bConnected == true) {
		m_emitter.erase(*m_mouseMove);
		m_mouseMove.reset();

		m_emitter.erase(*m_leftClickUp);
		m_leftClickUp.reset();

		m_emitter.erase(*m_leftClickDown);
		m_leftClickDown.reset();

		m_emitter.erase(*m_rightClickUp);
		m_rightClickUp.reset();

		m_emitter.erase(*m_rightClickDown);
		m_rightClickDown.reset();

		m_bConnected = false;
	}
}

void ISystem::onMouseMove(const evnt::MouseMove& event) {}

void ISystem::onLeftClickUp(const evnt::LeftClickUp& event) {}
void ISystem::onLeftClickDown(const evnt::LeftClickDown& event) {}

void ISystem::onRightClickUp(const evnt::RightClickUp& event) {}
void ISystem::onRightClickDown(const evnt::RightClickDown& event) {}