#include "random.hpp"
#include <random>
#include <ctime>

void initializeRandom() {
	srand(static_cast <unsigned> (time(0)));
}

float random(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

float random(float max) {
	return random(0, max);
}

float random() {
	return random(0, 1);
}

int randInt(int min, int maxPlusOne) {
	return floor(random(min, maxPlusOne));
}
int randInt(int maxPlusOne) {
	return randInt(0, maxPlusOne);
}