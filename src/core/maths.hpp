#pragma once

#include <glm/glm.hpp>

namespace imaths {
	float rangeMapping(float x, float in_min, float in_max, float out_min, float out_max);
	glm::vec2 segmentsIntersection(glm::vec2 P, glm::vec2 Q, glm::vec2 P_, glm::vec2 Q_);
	float min(float a, float b);
	float max(float a, float b);
	const float TAU = 6.28318530717958647692528676655900576839433879875021;
}
