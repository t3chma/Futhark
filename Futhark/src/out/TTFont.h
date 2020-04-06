#pragma once
#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <vector>
#include "SpriteBatch.h"
namespace fk {

// For text justification.
enum class Justification {
	LEFT,
	MIDDLE,
	RIGHT
};

class TTFont;

/* For managing text sprites.
[t3chma] */
class TextSprite {
  public:
	friend class TTFont;
	TextSprite(SpriteBatch& spriteBatch, TTFont& font);
	TextSprite(const TextSprite&) = default;
	TextSprite operator = (const TextSprite& rhs);
	void setPosition(glm::vec2 position, Justification m_justification);
	void setPosition(glm::vec2 position);
	void setDepth(int depth);
	void move(glm::vec2 translation);
	SpriteBatch::Sprite& operator [] (int charIndex);
	std::string getText();
	int getTextLength();
	void clearText();
	void setText(std::string text, glm::vec2 scale, Justification m_justification);
	void setText(std::string text);
  private:
	// The font of this class
	TTFont& m_font;
	// The string this textSprite represents.
	std::string m_string{""};
	// The sprite ids for each character.
	std::vector<int> m_spriteIds;
	// The spritebatch for the given character sprites.
	SpriteBatch& m_spriteBatch;
	// Base justification.
	Justification m_justification;
	// Base scale.
	glm::vec2 m_scale{ 1 };
};

/* Allows fonts to be displayed on screen.
[t3chma] */
class TTFont {
  public:
	TTFont() = default;
	/* Constructor.
	(fontFilePath) Filepath for this font.
	(size) The ptSize of this font.
	(firstASCIIChar) First ASCII character to be represented by this font.
	(lastASCIIChar) Last ASCII character to be represented by this font.
	[t3chma] */
	TTFont(const std::string& fontFilePath, int size = 64, char firstASCIIChar = 32, char lastASCIIChar = 126);
	// Returns the height of this font.
	inline int getHeight() const { return m_height; }
	/* Measures the dimensions of a string.
	(cString) The string to measure.
	[t3chma] */
	glm::vec2 measure(const char* cString);
	/* Make sprites for each leter some text in the given sprite batch.
	(text) The text to make sprites for.
	(spriteBatch) The sprite batch to add the sprites to.
	(size) The size of the generated text.
	< A wrapper class for handling the character sprites.
	[t3chma] */
	TextSprite generateCharSprites(
		const std::string text,
		SpriteBatch& spriteBatch,
		glm::vec2 scaling = glm::vec2(1),
		Justification m_justification = Justification::LEFT
	);
  private:
	/* Used to represent a single character in a sprite sheet.
	[t3chma] */
	struct m_Glyph {
		//  The character this glyph represents.
		char character;
		// The uv rectangle of this glyph on the sprite sheet.
		glm::vec4 uvRectangle;
		// The dimention of this glyph.
		glm::vec2 dimentions;
	};
	/* Partition the glyphs into rows.
	(glyphRects) The dimentions of each glyph.
	(numGlyphRects) How many glyphs there are.
	(rows) How many rows to generate.
	(padding) How much to pad each glyph out from one another.
	(width)< The final width of the partitionings.
	[t3chma] */
	std::vector<int>* m_createRows(
		glm::ivec4* glyphRects,
		int numGlyphRects,
		int rows,
		int padding,
		int& width
	);
	/* Get the closest power of two of the given number.
	(i) The number to get the closest power of.
	[t3chma] */
	int m_closestPow2(int i);
	// Which ASCII characters are used.
	int m_asciiStart{ 32 }, m_asciiRange{ 127 };
	// Array of glyoh info.
	m_Glyph* m_GlyphsPtr{ nullptr };
	// Height of each glyph.
	int m_height{ 64 };
	// The ID of the sprite sheet.
	GLuint m_textureID{ 0 };
};

}