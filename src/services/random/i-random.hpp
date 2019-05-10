#pragma once

class IRandom {
public:
	virtual float random(float min, float max) = 0;
	virtual float random(float max) = 0;
	virtual float random() = 0;

	virtual int randInt(int min, int maxPlusOne) = 0;
	virtual int randInt(int maxPlusOne) = 0;

	virtual float noise(float t) = 0;
};

