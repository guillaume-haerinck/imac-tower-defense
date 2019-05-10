#pragma once

#include "i-random.hpp"
#include <vector>

class RandomService : public IRandom {
public:
	RandomService();

	float random(float min, float max) override;
	float random(float max) override;
	float random() override;

	int randInt(int min, int maxPlusOne) override;
	int randInt(int maxPlusOne) override;

	float noise(float t);

private:
	int m_noiseNbPoints = 128;
	std::vector<float> m_noiseGradients;
	float m_noiseMaxGrad = 3;
	float m_noiseBlendFunction(float t);
};