#pragma once
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <boost/property_tree/ptree.hpp>
#include "Box2D/Box2D.h"
namespace fk {


/* A property tree.
^ http://www.boost.org/doc/libs/1_55_0/boost/property_tree/ptree.hpp */
using Dict = boost::property_tree::ptree;
/* Used to set app modes.
(PLAY) Normal operation mode.
(EXIT) Shuts down the app. */
enum class GameState {
	EXIT = 0,
	PLAY = 1
};


/* Random number generator.
[t3chma] */
class Random {
  public:
	/* Get a random int.
	(min) Minimum int to get back.
	(max) Maximum int to get back.
	< Random int in the given range.
	[t3chma] */
	int getInt(int min, int max) {
		return std::uniform_int_distribution<int>(min, max)(m_generator);
	}
	/* Get a random float.
	(min) Minimum float to get back.
	(max) Maximum float to get back.
	< Random float in the given range.
	[t3chma] */
	double getFloat(double min, double max) {
		return std::uniform_real_distribution<double>(min, max)(m_generator);
	}
  private:
	// The random number generator.
	std::mt19937 m_generator{ time(nullptr) };
};


// 2 PI, 1 Turn.
const double TAU = 3.14159265359 * 2;
/* Gets the angle of a vector.
(DIRECTION) The vector to get the angle of.
< The angle in radians.
[t3chma] */
inline float makeAngle(const glm::vec2& DIRECTION) {
	if (DIRECTION == glm::vec2(0, 0)) {
		return 0.0f;
	} else {
		float angle(acos(glm::dot(glm::vec2(1.0f, 0.0f), DIRECTION)));
		if (DIRECTION.y < 0.0f) angle = -angle;
		return angle;
	}
}
/* Rotates a point around the origin.
(POINT) The point to rotate.
(ANGLE) How much to rotate the point in radians.
[t3chma] */
inline glm::vec2 rotatePoint(const glm::vec2& POINT, const float ANGLE) {
	return glm::vec2(
		POINT.x * cos(ANGLE) - POINT.y * sin(ANGLE),
		POINT.x * sin(ANGLE) + POINT.y * cos(ANGLE)
	);
}
inline b2Vec2 rotatePoint(const b2Vec2& POINT, const float ANGLE) {
	return b2Vec2(
		POINT.x * cos(ANGLE) - POINT.y * sin(ANGLE),
		POINT.x * sin(ANGLE) + POINT.y * cos(ANGLE)
	);
}

}