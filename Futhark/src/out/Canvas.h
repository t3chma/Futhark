#pragma once
#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <GLEW/glew.h>
#include <boost/property_tree/ptree.hpp>
#include "../base/Utility.h"
namespace fk {


/* A UV coordinate container.
^ http://help.thefoundry.co.uk/nuke/8.0/content/user_guide/modelbuilder/uv_unwrapping.html
[t3chma] */
class UV {

public:

	float u;
	float v;

	UV() = default;

	/* Constructor
	(u) The horizontal texture mapping.
	(v) The vertical texture mapping.
	[t3chma] */
	UV(float u, float v);

};


	
/* A red, green, blue, alpha container.
[t3chma] */
class Color {

public:

	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;

	Color() = default;

	/* Constructor
	(r) Red value.
	(g) Green value.
	(b) Blue value.
	(a) Alpha value.
	[t3chma] */
	Color(GLubyte r, GLubyte g, GLubyte b, GLubyte a);

	Color operator + (const Color& COLOR);
	Color operator - (const Color& COLOR);
	Color operator * (const Color& COLOR);
	Color operator / (const Color& COLOR);
	Color operator + (const int& VALUE);
	Color operator - (const int& VALUE);
	Color operator * (const int& VALUE);
	Color operator / (const int& VALUE);
	bool operator == (const Color& COLOR);
	bool operator != (const Color& COLOR);

};

// Preset colors
namespace PresetColors {

	// 255, 0, 0, 255
	const Color RED(255, 0, 0, 255);
	// 255, 127, 127, 255
	const Color RED_LIGHT(255, 127, 127, 255);
	// 128, 0, 0, 255
	const Color RED_DARK(127, 0, 0, 255);

	// 255, 127, 0, 255
	const Color ORANGE(255, 127, 0, 255);
	// 127, 63, 0, 255
	const Color BROWN(127, 63, 0, 255);

	// 255, 255, 0, 255
	const Color YELLOW(255, 255, 0, 255);
	// 127, 127, 0, 255
	const Color YELLOW_DARK(127, 127, 0, 255);
	// 255, 255, 128, 255
	const Color YELLOW_LIGHT(255, 255, 127, 255);

	// 127, 255, 0, 255
	const Color LIME(127, 255, 0, 255);

	// 0, 255, 0, 255
	const Color GREEN(0, 255, 0, 255);
	// 0, 127, 0, 255
	const Color GREEN_DARK(0, 127, 0, 255);
	// 127, 255, 127, 255
	const Color GREEN_LIGHT(127, 255, 127, 255);

	// 0, 255, 127, 255
	const Color CREAM_GREEN(0, 255, 127, 255);

	// 0, 255, 255, 255
	const Color CYAN(0, 255, 255, 255);
	// 0, 127, 127, 255
	const Color CYAN_DARK(0, 127, 127, 255);
	// 127, 255, 255, 255
	const Color CYAN_LIGHT(127, 255, 255, 255);

	// 0, 127, 255, 255
	const Color COBALT(0, 127, 255, 255);

	// 0, 0, 255, 255
	const Color BLUE(0, 0, 255, 255);
	// 0, 0, 127, 255
	const Color BLUE_DARK(0, 0, 127, 255);
	// 127, 127, 255, 255
	const Color BLUE_LIGHT(127, 127, 255, 255);

	// 127, 0, 255, 255
	const Color PURPLE(127, 0, 255, 255);

	// 255, 0, 255, 255
	const Color MAROON(255, 0, 255, 255);
	// 127, 0, 127, 255
	const Color MAROON_DARK(127, 0, 127, 255);
	// 255, 127, 255, 255
	const Color MAROON_LIGHT(255, 127, 255, 255);

	// 255, 0, 127, 255
	const Color HOT_PINK(255, 0, 127, 255);

	// 255, 255, 255, 255
	const Color WHITE(255, 255, 255, 255);
	// 127, 127, 127, 255
	const Color GREY(127, 127, 127, 255);
	// 0, 0, 0, 255
	const Color BLACK(0, 0, 0, 255);

	// 0, 0, 0, 127
	const Color BLACK_TRANSLUCENT(0, 0, 0, 127);
	// 255, 255, 255, 127
	const Color WHITE_TRANSLUCENT(255, 255, 255, 127);

	// 0, 0, 0, 128
	// Subtract this from a color to make it translucent.
	const Color TRANSLUCENT_MODIFIER(0, 0, 0, 128);

	// 0, 0, 0, 0
	const Color INVISIBLE(0, 0, 0, 0);
}



/* Minimal vertex used for wireframes.
Contains 2 floats for position
and 4 bytes for RGBA color values.
[t3chma] */
class WFVertex {

public:

	// X Y coords.
	// 2 floats
	Position position;

	// RGBA color values from 0 - 255.
	// 4 bytes
	Color color;

	void setPos(const float X, const float Y);
	void setPos(const glm::vec2& POSITION);
	void setPos(const Position& POSITION);
	void setColor(const GLubyte R, const GLubyte G, const GLubyte B, const GLubyte A);
	void setColor(const Color& COLOR);
	void setPosColor(
		const float X, const float Y,
		const GLubyte R, const GLubyte G, const GLubyte B, const GLubyte A
	);
};



/* A container to pass into OpenGL for sprite rendering.
Contains 2 floats for position
4 bytes for RGBA color values
and 2 floats for UV coords.
[t3chma] */
class Vertex : public WFVertex {

public:

	// Texture UV coords
	// 2 floats
	UV uv;

	void setUV(const float U, const float V);
	void setUV(const glm::vec2& UV);
	void setUV(const UV& UV);
	void setPosColorUV(
		const float X, const float Y,
		const GLubyte R, const GLubyte G, const GLubyte B, const GLubyte A,
		const float U, const float V
	);
};



/* A container for sending to OpenGL for sprite rendering.
4 vertices
[t3chma] */
class Canvas {

public:

	Vertex topRight;
	Vertex topLeft;
	Vertex bottomRight;
	Vertex bottomLeft;

	Canvas() = default;

	/* Constructor
	(TOP_LEFT) Top left vertex.
	(TOP_RIGHT) Top right vertex.
	(BOTTOM_LEFT) Bottom left vertex.
	(BOTTOM_RIGHT) Bottom right vertex.
	[t3chma]*/
	Canvas(
		const Vertex& TOP_LEFT, const Vertex& TOP_RIGHT,
		const Vertex& BOTTOM_LEFT, const Vertex& BOTTOM_RIGHT
	);

	/* Constructor
	(POSITION) Position if the bottom left corner.
	(DIMENSIONS) The width and height.
	(COLOR) Color of the entire canvas.
	(UV_RECT) Rectangulare UV mapping of the texture.
	[t3chma]
	^ http://help.thefoundry.co.uk/nuke/8.0/content/user_guide/modelbuilder/uv_unwrapping.html
	[t3chma] */
	Canvas(
		const glm::vec2& POSITION,
		const glm::vec2& DIMENSIONS,
		const Color& COLOR = PresetColors::WHITE,
		const glm::vec4& UV_RECT = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
	);

	/* Constructor
	(X) X position of the bottom left corner.
	(Y) y position of the bottom left corner.
	(WIDTH) Width of the canvas.
	(HEIGHT) Height of the canvas.
	(COLOR) Color of the entire canvas.
	(UV_RECT) Rectangulare UV mapping of the texture.
	[t3chma]
	^ http://help.thefoundry.co.uk/nuke/8.0/content/user_guide/modelbuilder/uv_unwrapping.html */
	Canvas(
		const float X, const float Y,
		const float WIDHT, const float HEIGHT,
		const Color& COLOR = PresetColors::WHITE,
		const glm::vec4& UV_RECT = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
	);

	void move(const glm::vec2& TRANSLATION);
	void move(const Position& TRANSLATION);
	void move(const float X, const float Y);
	void resize(const glm::vec2& DIMENSIONS);
	void resize(const float WIDTH, const float HEIGHT);
	void setColor(const Color& COLOR);
	void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
	glm::vec2 getDimensions() const;

	/* Rotates canvas around a point.
	(rotantionPointPTR) A constant pointer to the point to rotate around.
	nullptr means rotate around the center.
	(ANGLE) The amount to rotate in radians.
	[t3chma] */
	void rotate(glm::vec2* const rotantionPointPTR, const float ANGLE);
};

}