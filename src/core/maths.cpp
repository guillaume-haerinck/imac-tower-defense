#include "maths.hpp"
#include <limits>

float imaths::rangeMapping(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

glm::vec2 imaths::segmentsIntersection(glm::vec2 P, glm::vec2 Q, glm::vec2 P_, glm::vec2 Q_)
{//Returns t and t_ such that the intersection point is equal to P+t*PQ and P_+t_*P_Q_.
//(note : there is an actual intersection iff 0 <= t <= 1 and 0 <= t_ <= 1
	float det = (Q.x - P.x)*(P_.y - Q_.y) - (Q.y - P.y)*(P_.x - Q_.x);
	if (det == 0) {
		return glm::vec2(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()); // En vrai faut tester si les droites sont confondues
	}
	else {
		float t = ((P_.y - Q_.y)*(P_.x - P.x) + (Q_.x - P_.x)*(P_.y - P.y)) / det;
		float t_ = ((P.y - Q.y)*(P_.x - P.x) + (Q.x - P.x)*(P_.y - P.y)) / det;
		return glm::vec2(t,t_);
	}
}

float imaths::distance(glm::vec2 v1, glm::vec2 v2) {
	return sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
}

float imaths::distanceSq(glm::vec2 v1, glm::vec2 v2) {
	return (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y);
}


float imaths::min(float a, float b) {
	return a < b ? a : b;
}

float imaths::max(float a, float b) {
	return a > b ? a : b;
}