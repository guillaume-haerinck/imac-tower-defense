#include "progression.hpp"

#include "events/progression-updated.hpp"

Progression::Progression(EventEmitter& emitter) : m_money(50), m_emitter(emitter) {}

Progression::~Progression() {}

/* ------------------------ GETTERS ---------------------- */

int Progression::getMoney() { return m_money; }

/* ---------------------------- SETTERS --------------------------- */

void Progression::addToMoney(int amount) {
	if (amount += m_money >= 0) {
		m_money += amount;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
