#pragma once
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <boost/property_tree/ptree.hpp>
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
	static int getInt(int min, int max);
	/* Get a random float.
	(min) Minimum float to get back.
	(max) Maximum float to get back.
	< Random float in the given range.
	[t3chma] */
	static double getFloat(double min, double max);
  private:
	// The random number generator.
	static std::mt19937 m_generator;
};


// 2 PI, 1 Turn.
const double TAU = 3.14159265359 * 2;
/* Gets the angle of a vector.
(DIRECTION) The vector to get the angle of.
< The angle in radians.
[t3chma] */
extern float makeAngle(const glm::vec2& DIRECTION);
/* Rotates a point around the origin.
(POINT) The point to rotate.
(ANGLE) How much to rotate the point in radians.
[t3chma] */
extern glm::vec2 rotatePoint(const glm::vec2& POINT, const float ANGLE);

}