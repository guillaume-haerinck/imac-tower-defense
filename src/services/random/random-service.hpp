#pragma once

#include "i-random.hpp"

class RandomService : public IRandom {
public:
	RandomService();

	float random(float min, float max) override;
	float random(float max) override;
	float random() override;

	int randInt(int min, int maxPlusOne) override;
	int randInt(int maxPlusOne) override;
};