#pragma once
#include <GLEW/glew.h>
#include <GLM/glm.hpp>
#include <vector>
namespace fk {
class SpriteBatch;

//class TextSprite {
//  public:
//	friend class TTFont;
//	TextSprite(SpriteBatch spriteBatch);
//	void setPosition(glm::vec2 position);
//	void move(glm::vec2 translation);
//	SpriteBatch::Sprite& getCharSprite(int charIndex);
//	std::string getText();
//  private:
//	// The string this textSprite represents.
//	std::string m_string;
//	// The sprite ids for each character.
//	std::vector<int> m_spriteIds;
//	// The spritebatch for the given character sprites.
//	SpriteBatch m_spriteBatch;
//};

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
	TTFont(const std::string fontFilePath, int size = 64, char firstASCIIChar = 32, char lastASCIIChar = 126);
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
	void generateCharSprites(
		const std::string text,
		SpriteBatch& spriteBatch,
		glm::vec2 size = glm::vec2(1)
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
	// Which ASCII characters are.
	int m_asciiStart, m_asciiRange;
	// Array of glyoh info.
	m_Glyph* m_GlyphsPtr;
	// Height of each glyph.
	int m_height;
	// The ID of the sprite sheet.
	GLuint m_textureID;
};

}