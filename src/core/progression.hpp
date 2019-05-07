#pragma once

#include "events/handlers/event-emitter.hpp"

class Progression {
public:
	Progression(EventEmitter& emitter);
	~Progression();

	// Getters
	int getMoney();

	// Setters
	void addToMoney(int amount);

private:
	EventEmitter& m_emitter;
	int m_money;
};
