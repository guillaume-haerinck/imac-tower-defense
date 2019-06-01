#pragma once
#include "core/maths.hpp"

namespace cmpt {
struct ConstrainedRotation {
	//Entity can only have an angle of k/nbAngles*TAU where 0 <= k < nbAngles
	ConstrainedRotation(int nbAngles) : nbAngles(nbAngles), angleStep(imaths::TAU/nbAngles), accumulatedAngle(0), angleIndex(0) {}

	int nbAngles;
    float angleStep;
	float accumulatedAngle;
	int angleIndex;
};
}
