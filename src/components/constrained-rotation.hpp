#pragma once
#include "core/maths.hpp"

namespace cmpt {
struct ConstrainedRotation {
	ConstrainedRotation(int nbAngles) : nbAngles(nbAngles), angleStep(imac::TAU/nbAngles), accumulatedAngle(0) {}

	int nbAngles;
    float angleStep;
	float accumulatedAngle;
};
}
