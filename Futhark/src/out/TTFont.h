#pragma once
#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <vector>
namespace fk {
class SpriteBatch;


/* Allows fonts to be displayed on screen.
[t3chma] */
class TTFont {
public:
	// For text justification.
	enum class Justification {
		LEFT,
		MIDDLE,
		RIGHT
	};
	TTFont() = default;
	/* Constructor.
	(fontFilePath) Filepath for this font.
	(size) The ptSize of this font.
	(firstASCIIChar) First ASCII character to be represented by this font.
	(lastASCIIChar) Last ASCII character to be represented by this font.
	[t3chma] */
	TTFont(const std::string fontFilePath, int size = 64, char firstASCIIChar = 32, char lastASCIIChar = 64);
	// Returns the height of this font.
	inline int getHeight() const { return m_height; }
	/* Measures the dimensions of a string.
	(cString) The string to measure.
	[t3chma] */
	glm::vec2 measure(const char* cString);
	/* 
	[t3chma] */
	void generateCharSprites(const char* cString, SpriteBatch& spriteBatch, glm::vec2 scaling);
private:

	/*
	[t3chma] */
	struct m_Glyph {
		char character;
		glm::vec4 uvRectangle;
		glm::vec2 dimentions;
	};

	// Partition the glyphs into rows.
	std::vector<int>* m_createRows(
		glm::ivec4 * glyphRects,
		int numGlyphRects,
		int rows,
		int padding,
		int& width
	);
	// Get the closest power of two of the given number.
	int m_closestPow2(int i);
	// Which ASCII characters are.
	int m_asciiStart, m_asciiRange;
	m_Glyph* m_GlyphsPtr;
	// Height of each glyph.
	int m_height;
	// The ID of the sprite sheet.
	GLuint m_textureID;
};

}