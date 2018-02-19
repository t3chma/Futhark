#pragma once
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <boost/property_tree/ptree.hpp>

#ifdef __PRINT_STEPS__
#include <iostream>
#endif

namespace fk {

/* A property tree.
^ http://www.boost.org/doc/libs/1_55_0/boost/property_tree/ptree.hpp */
typedef boost::property_tree::ptree Dict;

/* Used to set app modes.
(PLAY) Normal operation mode.
(EXIT) Shuts down the app. */
enum class GameState {
	EXIT = 0,
	PLAY = 1
};

/* Make a char* buffer from a string.
(string) The string to convert.
< A pair with the char* buffer and its size.
[t3chma] */
extern std::pair<char*, int> makeCharStarBuffer(const std::string& STRING);


/* Random number generator.
[t3chma] */
class Random {

public:

	/* Get a random int.
	(min) Mininum int to get back.
	(max) Maximum int to get back.
	< Random int in the given range.
	[t3chma] */
	inline static int getInt(int min, int max) {
		return std::uniform_int_distribution<int>(min, max)(m_generator);
	}

	/* Get a random float.
	(min) Mininum float to get back.
	(max) Maximum float to get back.
	< Random float in the given range.
	[t3chma] */
	inline static double getFloat(double min, double max) {
		return std::uniform_real_distribution<double>(min, max)(m_generator);
	}

private:

	// The random number generator.
	static std::mt19937 m_generator;
};

/* An X Y coordinate container.
[t3chma] */
class Position {

public:

	float x;
	float y;

	Position() = default;

	/* Constructor
	(x) X coord.
	(y) Y coord.
	[t3chma] */
	Position(float x, float y);

	void operator += (const Position& POSITION);
	void operator -= (const Position& POSITION);
	Position operator + (const Position& POSITION);
	Position operator - (const Position& POSITION);
	void operator += (const glm::vec2& POSITION);
	void operator -= (const glm::vec2& POSITION);
	Position operator + (const glm::vec2& POSITION);
	Position operator - (const glm::vec2& POSITION);

	/* Rotates a point around the origin.
	(ANGLE) How much to rotate the point in radians.
	[t3chma] */
	void rotate(const float ANGLE);
};

/* A container for sending to OpenGL for sprite rendering.
4 glm::vec2s
[t3chma] */
class Rectangle {

public:

	glm::vec2 topRight;
	glm::vec2 topLeft;
	glm::vec2 bottomLeft;
	glm::vec2 bottomRight;

	Rectangle() = default;

	/* Constructor
	(topRight) Top right vector.
	(topLeft) Top left vector.
	(bottomLeft) Bottom left vector.
	(bottomRight) Bottom right vector.
	[t3chma] */
	Rectangle(
		const glm::vec2& TOP_RIGHT,
		const glm::vec2& TOP_LEFT,
		const glm::vec2& BOTTOM_LEFT,
		const glm::vec2& BOTTOM_RIGHT
	);

	/* Constructor
	(left) Left most X coord.
	(bottom) Bottom most Y coord.
	(width) Width of the rectangle.
	(height) Height of the rectangle.
	[t3chma] */
	Rectangle(
		float left,
		float bottom,
		float width,
		float height
	);

	/* Rotates rectangle around a point.
	(rotantionPointPTR) A constant pointer to the point to rotate around.
	nullptr means rotate around the center.
	(ANGLE) The amount to rotate in radians.
	[t3chma] */
	void rotate(glm::vec2* const rotantionPointPTR, const float& ANGLE);
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