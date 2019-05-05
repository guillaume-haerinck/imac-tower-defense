#include "progression.hpp"

Progression::Progression() : m_money(50) {}

Progression::~Progression() {}

/* ------------------------ GETTERS ---------------------- */

unsigned int Progression::getMoney() { return m_money; }

/* ---------------------------- SETTERS --------------------------- */

void Progression::addToMoney(int amount) {
	if (amount += m_money >= 0) {
		m_money += amount;
	}
}
