#include "random-service.hpp"

#include <random>
#include <ctime>

#include <spdlog/spdlog.h>

RandomService::RandomService() {
	//Initialize rand
	srand(static_cast <unsigned> (time(0)));
	//Initialize noise
	m_noiseGradients.reserve(m_noiseNbPoints);
	for (int i = 0; i < m_noiseNbPoints; ++i) {
		m_noiseGradients.push_back(random(-m_noiseMaxGrad, m_noiseMaxGrad));
	}
}

float RandomService::random(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

float RandomService::random(float max) {
	return random(0, max);
}

float RandomService::random() {
	return random(0, 1);
}

int RandomService::randInt(int min, int maxPlusOne) {
	return floor(random(min, maxPlusOne));
}

int RandomService::randInt(int maxPlusOne) {
	return randInt(0, maxPlusOne);
}

// Noise, using this reference http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf and this https://github.com/SRombauts/SimplexNoise
float RandomService::m_noiseBlendFunction(float t) {
	return t*t*t*(t*(t * 6 - 15) + 10);
}

float RandomService::noise(float t) {
	int n0 = floor(t) ;
	t -= n0;
	n0 = n0 % m_noiseNbPoints;
	int n1 = (n0 + 1) % m_noiseNbPoints;
	float r0 = m_noiseGradients.at(n0) * t;
	float r1 = m_noiseGradients.at(n1) * (1-t);
	float blendFactor = m_noiseBlendFunction(t);
	float r = r0*(1 - blendFactor) + r1*blendFactor;
	return r; 
}