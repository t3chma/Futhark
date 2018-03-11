#include "Utility.h"

namespace fk {
	

	std::mt19937 Random::m_generator{ (unsigned int)time(nullptr) };
	int Random::getInt(int min, int max) {
		return std::uniform_int_distribution<int>(min, max)(m_generator);
	}
	double Random::getFloat(double min, double max) {
		return std::uniform_real_distribution<double>(min, max)(m_generator);
	}

	float makeAngle(const glm::vec2& DIRECTION) {
		if (DIRECTION == glm::vec2(0, 0)) {
			return 0.0f;
		} else {
			float angle(acos(glm::dot(glm::vec2(1.0f, 0.0f), DIRECTION)));
			if (DIRECTION.y < 0.0f) angle = -angle;
			return angle;
		}
	}
	glm::vec2 rotatePoint(const glm::vec2& POINT, const float ANGLE) {
		return glm::vec2(
			POINT.x * cos(ANGLE) - POINT.y * sin(ANGLE),
			POINT.x * sin(ANGLE) + POINT.y * cos(ANGLE)
		);
	}
}