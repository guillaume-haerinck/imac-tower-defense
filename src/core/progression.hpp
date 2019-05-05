#pragma once

class Progression {
public:
	Progression();
	~Progression();

	// Getters
	unsigned int getMoney();

	// Setters
	void addToMoney(int amount);

private:
	unsigned int m_money;
};
