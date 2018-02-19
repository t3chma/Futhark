#include "Utility.h"

namespace fk {
	
	std::mt19937 Random::m_generator{ (unsigned int)time(nullptr) };

	float makeAngle(const glm::vec2& DIRECTION) {
		if (DIRECTION == glm::vec2(0, 0)) {
			return 0.0f;
		}
		else {
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

	void Position::rotate(const float ANGLE) {
		x = x * cos(ANGLE) - y * sin(ANGLE);
		y = x * sin(ANGLE) + y * cos(ANGLE);
	}

	Rectangle::Rectangle(
		const glm::vec2& TOP_RIGHT,
		const glm::vec2& TOP_LEFT,
		const glm::vec2& BOTTOM_LEFT,
		const glm::vec2& BOTTOM_RIGHT
	)
		: topRight(TOP_RIGHT), topLeft(TOP_LEFT), bottomLeft(BOTTOM_LEFT), bottomRight(BOTTOM_RIGHT) {
	}

	Rectangle::Rectangle(float left, float bottom, float width, float height)
		: topRight(left + width, bottom + height),
		topLeft(left, bottom + height),
		bottomLeft(left, bottom),
		bottomRight(left + width, bottom) {
	}



	void Rectangle::rotate(
		glm::vec2* const rotantionPointPTR,
		const float& ANGLE
	) {
		if (ANGLE != 0) {
			if (rotantionPointPTR == nullptr) {
				glm::vec2 rotationPoint(
					bottomLeft.x + (topRight.x - bottomLeft.x) / 2,
					bottomLeft.y + (topRight.y - bottomLeft.y) / 2
				);

				// Move rectangle to origin with respect to the rotation point
				topRight -= rotationPoint;
				topLeft -= rotationPoint;
				bottomLeft -= rotationPoint;
				bottomRight -= rotationPoint;
				// Rotate the points
				topRight = rotatePoint(topRight, ANGLE);
				topLeft = rotatePoint(topLeft, ANGLE);
				bottomLeft = rotatePoint(bottomLeft, ANGLE);
				bottomRight = rotatePoint(bottomRight, ANGLE);
				// Move rectangle back from the origin
				topRight += rotationPoint;
				topLeft += rotationPoint;
				bottomLeft += rotationPoint;
				bottomRight += rotationPoint;
			}
			else {
				// Move rectangle to origin with respect to the rotation point
				topRight = topRight - *rotantionPointPTR;
				topLeft = topLeft - *rotantionPointPTR;
				bottomLeft = bottomLeft - *rotantionPointPTR;
				bottomRight = bottomRight - *rotantionPointPTR;
				// Rotate the points
				topRight = rotatePoint(topRight, ANGLE);
				topLeft = rotatePoint(topLeft, ANGLE);
				bottomLeft = rotatePoint(bottomLeft, ANGLE);
				bottomRight = rotatePoint(bottomRight, ANGLE);
				// Move rectangle back from the origin
				topRight = topRight + *rotantionPointPTR;
				topLeft = topLeft + *rotantionPointPTR;
				bottomLeft = bottomLeft + *rotantionPointPTR;
				bottomRight = bottomRight + *rotantionPointPTR;
			}
		}
	}

	Position::Position(float x, float y) { this->x = x; this->y = y; }

	void Position::operator += (const Position& POSITION) { this->x += POSITION.x; this->y += POSITION.y; }

	void Position::operator -= (const Position& POSITION) { this->x -= POSITION.x; this->y -= POSITION.y; }

	Position Position::operator + (const Position& POSITION) {
		return Position(this->x + POSITION.x, this->y + POSITION.y);
	}

	Position Position::operator - (const Position& POSITION) {
		return Position(this->x - POSITION.x, this->y - POSITION.y);
	}

	void Position::operator += (const glm::vec2& POSITION) { this->x += POSITION.x; this->y += POSITION.y; }

	void Position::operator -= (const glm::vec2& POSITION) { this->x -= POSITION.x; this->y -= POSITION.y; }

	Position Position::operator + (const glm::vec2& POSITION) {
		return Position(this->x + POSITION.x, this->y + POSITION.y);
	}

	Position Position::operator - (const glm::vec2& POSITION) {
		return Position(this->x - POSITION.x, this->y - POSITION.y);
	}

	std::pair<char*, int> makeCharStarBuffer(const std::string& STRING) {
		char* charArray = new char[STRING.length() + 1];
		int i = 0;
		for (auto&& e : STRING) { charArray[i++] = e; }
		charArray[i] = '\0';
		return std::pair<char*, int>(charArray, STRING.length() + 1);
	}
}