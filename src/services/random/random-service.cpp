#include "random-service.hpp"

#include <random>
#include <ctime>

RandomService::RandomService() {
	srand(static_cast <unsigned> (time(0)));
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
