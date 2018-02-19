#include "Canvas.h"
#include "../base/Utility.h"
namespace fk {

	void Canvas::rotate(glm::vec2* const rotantionPointPTR, const float ANGLE) {
		if (ANGLE != 0) {
			if (rotantionPointPTR == nullptr) {
				glm::vec2 rotationPoint(
					bottomLeft.position.x + (topRight.position.x - bottomLeft.position.x) / 2,
					bottomLeft.position.y + (topRight.position.y - bottomLeft.position.y) / 2
				);

				// Move rectangle to origin with respect to the rotation point
				topRight.position -= rotationPoint;
				topLeft.position -= rotationPoint;
				bottomLeft.position -= rotationPoint;
				bottomRight.position -= rotationPoint;
				// Rotate the points
				topRight.position.rotate(ANGLE);
				topLeft.position.rotate(ANGLE);
				bottomLeft.position.rotate(ANGLE);
				bottomRight.position.rotate(ANGLE);
				// Move rectangle back from the origin
				topRight.position += rotationPoint;
				topLeft.position += rotationPoint;
				bottomLeft.position += rotationPoint;
				bottomRight.position += rotationPoint;
			}
			else {
				// Move rectangle to origin with respect to the rotation point
				topRight.position -= *rotantionPointPTR;
				topLeft.position -= *rotantionPointPTR;
				bottomLeft.position -= *rotantionPointPTR;
				bottomRight.position -= *rotantionPointPTR;
				// Rotate the points
				topRight.position.rotate(ANGLE);
				topLeft.position.rotate(ANGLE);
				bottomLeft.position.rotate(ANGLE);
				bottomRight.position.rotate(ANGLE);
				// Move rectangle back from the origin
				topRight.position += *rotantionPointPTR;
				topLeft.position += *rotantionPointPTR;
				bottomLeft.position += *rotantionPointPTR;
				bottomRight.position += *rotantionPointPTR;
			}
		}
	}

	Canvas::Canvas(
		const Vertex& TOP_LEFT, const Vertex& TOP_RIGHT, const Vertex& BOTTOM_LEFT, const Vertex& BOTTOM_RIGHT
	) : topLeft(TOP_LEFT), topRight(TOP_RIGHT), bottomLeft(BOTTOM_LEFT), bottomRight(BOTTOM_RIGHT) {
	}

	Canvas::Canvas(
		const glm::vec2& POSITION, const glm::vec2& DIMENSIONS,
		const Color& COLOR,
		const glm::vec4& UV_RECTANGLE
	) {
		bottomLeft.setPos(POSITION);
		topRight.setPos(POSITION + DIMENSIONS);
		bottomRight.setPos(POSITION.x + DIMENSIONS.x, POSITION.y);
		topLeft.setPos(POSITION.x, POSITION.y + DIMENSIONS.y);
		setColor(COLOR);
		bottomLeft.setUV(UV_RECTANGLE.x, UV_RECTANGLE.y);
		topRight.setUV(UV_RECTANGLE.x + UV_RECTANGLE.z, UV_RECTANGLE.y + UV_RECTANGLE.w);
		bottomRight.setUV(UV_RECTANGLE.x + UV_RECTANGLE.z, UV_RECTANGLE.y);
		topLeft.setUV(UV_RECTANGLE.x, UV_RECTANGLE.y + UV_RECTANGLE.w);
	}

	Canvas::Canvas(
		const float X_POSITION, const float Y_POSITION,
		const float WIDTH, const float HEIGHT,
		const Color& COLOR, const glm::vec4& UV_RECTANGLE
	){
		bottomLeft.setPos(X_POSITION, Y_POSITION);
		topRight.setPos(X_POSITION + WIDTH, Y_POSITION + HEIGHT);
		bottomRight.setPos(X_POSITION + WIDTH, Y_POSITION);
		topLeft.setPos(X_POSITION, Y_POSITION + HEIGHT);
		setColor(COLOR);
		bottomLeft.setUV(UV_RECTANGLE.x, UV_RECTANGLE.y);
		topRight.setUV(UV_RECTANGLE.x + UV_RECTANGLE.z, UV_RECTANGLE.y + UV_RECTANGLE.w);
		bottomRight.setUV(UV_RECTANGLE.x + UV_RECTANGLE.z, UV_RECTANGLE.y);
		topLeft.setUV(UV_RECTANGLE.x, UV_RECTANGLE.y + UV_RECTANGLE.w);
	}

	void Canvas::move(const glm::vec2& TRANSLATION) {
		topRight.position += TRANSLATION;
		topLeft.position += TRANSLATION;
		bottomRight.position += TRANSLATION;
		bottomLeft.position += TRANSLATION;
	}

	void Canvas::move(const Position& TRANSLATION) {
		topRight.position += TRANSLATION;
		topLeft.position += TRANSLATION;
		bottomRight.position += TRANSLATION;
		bottomLeft.position += TRANSLATION;
	}

	void Canvas::move(const float X, const float Y) {
		topRight.position.x += X;
		topLeft.position.x += X;
		bottomRight.position.x += X;
		bottomLeft.position.x += X;

		topRight.position.y += Y;
		topLeft.position.y += Y;
		bottomRight.position.y += Y;
		bottomLeft.position.y += Y;
	}

	void Canvas::resize(const glm::vec2& DIMENSIONS) {
		topRight.position.x = bottomLeft.position.x + DIMENSIONS.x;
		topRight.position.y = bottomLeft.position.y + DIMENSIONS.y;
		bottomRight.position.x = bottomLeft.position.x + DIMENSIONS.x;
		topLeft.position.y = bottomLeft.position.y + DIMENSIONS.y;
	}

	void Canvas::resize(const float WIDTH, const float HEIGHT) {
		topRight.position.x = bottomLeft.position.x + WIDTH;
		topRight.position.y = bottomLeft.position.y + HEIGHT;
		bottomRight.position.x = bottomLeft.position.x + WIDTH;
		topLeft.position.y = bottomLeft.position.y + HEIGHT;
	}

	void Canvas::setColor(const Color& COLOR) {
		topRight.setColor(COLOR);
		topLeft.setColor(COLOR);
		bottomRight.setColor(COLOR);
		bottomLeft.setColor(COLOR);
	}

	void Canvas::setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
		topRight.setColor(r, g, b, a);
		topLeft.setColor(r, g, b, a);
		bottomRight.setColor(r, g, b, a);
		bottomLeft.setColor(r, g, b, a);
	}

	glm::vec2 Canvas::getDimensions() const {
		return glm::vec2(
			bottomLeft.position.x - topRight.position.x,
			bottomLeft.position.y - topRight.position.y
		);
	}

	UV::UV(float u, float v) : u(u), v(v) {}

	Color::Color(GLubyte r, GLubyte g, GLubyte b, GLubyte a) : r(r), g(g), b(b), a(a) {}

	Color Color::operator + (const Color& COLOR) {
		return Color(this->r + COLOR.r, this->g + COLOR.g, this->b + COLOR.b, this->a + COLOR.a);
	}

	Color Color::operator - (const Color& COLOR) {
		return Color( this->r - COLOR.r, this->g - COLOR.g, this->b - COLOR.b, this->a - COLOR.a);
	}

	Color Color::operator * (const Color& COLOR){
		return Color(this->r * COLOR.r, this->g * COLOR.g, this->b * COLOR.b, this->a);
	}

	Color Color::operator / (const Color& COLOR) {
		return Color(this->r / COLOR.r, this->g / COLOR.g, this->b / COLOR.b, this->a);
	}

	Color Color::operator + (const int& VALUE) {
		return Color(this->r + VALUE, this->g + VALUE, this->b + VALUE, this->a + VALUE);
	}

	Color Color::operator - (const int& VALUE) {
		return Color(this->r - VALUE, this->g - VALUE, this->b - VALUE, this->a - VALUE);
	}

	Color Color::operator * (const int& VALUE) {
		return Color(this->r * VALUE, this->g * VALUE, this->b * VALUE, this->a);
	}

	Color Color::operator / (const int& VALUE) {
		return Color(this->r / VALUE, this->g / VALUE, this->b / VALUE, this->a);
	}

	bool Color::operator == (const Color& COLOR) {
		if (this->r == COLOR.r) {
			if (this->g == COLOR.g) {
				if (this->b == COLOR.b) {
					if (this->a == COLOR.a) {
						return true;
					}
				}
			}
		}
		return false;
	}

	bool Color::operator != (const Color& COLOR) {
		if (this->r != COLOR.r) { return true; }
		if (this->g != COLOR.g) { return true; }
		if (this->b != COLOR.b) { return true; }
		if (this->a != COLOR.a) { return true; }
		return false;
	}

	void WFVertex::setPos(const float X, const float Y) {
		position.x = X; position.y = Y;
	}
	void WFVertex::setPos(const glm::vec2& POSITION) {
		this->position.x = POSITION.x; this->position.y = POSITION.y;
	}

	void WFVertex::setPos(const Position& POSITION) {
		this->position = POSITION;
	}
	void WFVertex::setColor(const GLubyte R, const GLubyte G, const GLubyte B, const GLubyte A) {
		color.r = R; color.g = G; color.b = B; color.a = A;
	}

	void WFVertex::setColor(const Color& COLOR) {
		this->color = COLOR;
	}

	void WFVertex::setPosColor(
		const float x, const float y,
		const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte a) {
		setPos(x, y); setColor(r, g, b, a);
	}

	void Vertex::setUV(const float U, const float V) { uv.u = U; uv.v = V; }
	
	void Vertex::setUV(const glm::vec2& UV) { this->uv.u = UV.x; this->uv.v = UV.y; }
	
	void Vertex::setUV(const UV& UV) { this->uv = UV; }
	
	void Vertex::setPosColorUV(const float X, const float Y, const GLubyte R, const GLubyte G, const GLubyte B, const GLubyte A, const float U, const float V) {
		setPos(X, Y); setColor(R, G, B, A); setUV(U, V);
	}
}