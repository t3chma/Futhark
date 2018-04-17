#include "TTFont.h"
#include <SDL/TTF/SDL_ttf.h>
#include "Error.h"
#include "SpriteBatch.h"
namespace fk {


int TTFont::m_closestPow2(int i) {
	i--;
	int pi = 1;
	while (i > 0) { i >>= 1; pi <<= 1; }
	return pi;
}
std::vector<int>* TTFont::m_createRows(
	glm::ivec4* glyphRects,
	int numGlyphRects,
	int rows,
	int padding,
	int& width
) {
	// Blank initialize.
	std::vector<int>* partitions = new std::vector<int>[rows]();
	std::vector<int> rowWidths;
	rowWidths.resize(rows);
	// Add the front padding to each row.
	for (auto&& rowWidth : rowWidths) { rowWidth = padding; }
	// Loop through all glyphs.
	for (auto glyphRect = 0; glyphRect < numGlyphRects; glyphRect++) {
		// Find row for placement.
		int rowIndex = 0;
		for (auto compareRowIndex = 1; compareRowIndex < rows; compareRowIndex++) {
			if (rowWidths[compareRowIndex] < rowWidths[rowIndex]) { rowIndex = compareRowIndex; }
		}
		// Add width to that row.
		rowWidths[rowIndex] += glyphRects[glyphRect].z + padding;
		// Add glyph to the row list.
		partitions[rowIndex].push_back(glyphRect);
	}
	// Find the max width and return the partition info.
	width = 0;
	for (auto&& rowWidth : rowWidths) { if (rowWidth > width) { width = rowWidth; } }
	return partitions;
}

TTFont::TTFont(const std::string fontFilePath, int size, char firstASCIIChar, char lastASCIIChar) {
	// Open a font https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_14.html
	TTF_Font* fontPtr = TTF_OpenFont(fontFilePath.c_str(), size);
	if (fontPtr == nullptr) {
		LOG_LINE(std::string("Failed to open TTF font: " + fontFilePath).c_str());
		BREAK_IF(true);
	}
	// Obtain height https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_39.html
	m_height = TTF_FontHeight(fontPtr);
	m_asciiStart = firstASCIIChar;
	m_asciiRange = lastASCIIChar - firstASCIIChar + 1;
	int padding = size / 8;
	// First measure all the regions.
	glm::ivec4* glyphRects = new glm::ivec4[m_asciiRange];
	int i = 0, advance;
	for (auto c = firstASCIIChar; c <= lastASCIIChar; c++) {
		// Get glyph metrics https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_38.html
		TTF_GlyphMetrics(
			fontPtr,
			c,
			&glyphRects[i].x,
			&glyphRects[i].z,
			&glyphRects[i].y,
			&glyphRects[i].w,
			&advance
		);
		glyphRects[i].z -= glyphRects[i].x;
		glyphRects[i].x = 0;
		glyphRects[i].w -= glyphRects[i].y;
		glyphRects[i].y = 0;
		i++;
	}
	// Find best partitioning of glyphs for the font sheet.
	int rows = 1, width, height, bestWidth = 0, bestHeight = 0, area = 4096 * 4096, bestRows = 0;
	std::vector<int>* bestPartitions = nullptr;
	while (rows <= m_asciiRange) {
		height = rows * (padding + m_height) + padding;
		std::vector<int>* partitions = m_createRows(glyphRects, m_asciiRange, rows, padding, width);
		// Desire a power of 2 texture
		width = m_closestPow2(width);
		height = m_closestPow2(height);
		// A texture must be feasible
		if (width > 4096 || height > 4096) {
			rows++;
			delete[] partitions;
			continue;
		}
		// Check for minimal area
		if (area >= width * height) {
			if (bestPartitions) { delete[] bestPartitions; }
			bestPartitions = partitions;
			bestWidth = width;
			bestHeight = height;
			bestRows = rows;
			area = bestWidth * bestHeight;
			rows++;
		} else {
			delete[] partitions;
			break;
		}
	}
	if (!bestPartitions) {
		LOG_LINE(
			std::string("Failed to Map TTF font to texture. Try lowering resolution: " + fontFilePath).c_str()
		);
	}
	// Create the texture.
	TRY_GL(glGenTextures(1, &m_textureID));
	TRY_GL(glBindTexture(GL_TEXTURE_2D, m_textureID));
	TRY_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bestWidth, bestHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	// Now draw all the glyphs.
	SDL_Color color = { 255, 255, 255, 255 };
	int yOffset = padding;
	for (auto rowIndex = 0; rowIndex < bestRows; rowIndex++) {
		int xOffset = padding;
		for (auto charIndex = 0; charIndex < bestPartitions[rowIndex].size(); charIndex++) {
			int glyphIndex = bestPartitions[rowIndex][charIndex];
			SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(
				fontPtr,
				(char)(firstASCIIChar + glyphIndex),
				color
			);
			// Pre-multiplication occurs here
			unsigned char* surfacePixels = (unsigned char*)glyphSurface->pixels;
			int colorIndecies = glyphSurface->w * glyphSurface->h * 4;
			for (auto colorIndex = 0; colorIndex < colorIndecies; colorIndex += 4) {
				float alpha = surfacePixels[colorIndex + 3] / 255.0f;
				surfacePixels[colorIndex] *= alpha;
				surfacePixels[colorIndex + 1] = surfacePixels[colorIndex];
				surfacePixels[colorIndex + 2] = surfacePixels[colorIndex];
			}
			// Save glyph image and update coordinates
			// https://www.opengl.org/sdk/docs/man/html/glTexSubImage2D.xhtml
			TRY_GL(glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				xOffset,
				bestHeight - yOffset - 1 - glyphSurface->h,
				glyphSurface->w,
				glyphSurface->h,
				GL_BGRA,
				GL_UNSIGNED_BYTE,
				glyphSurface->pixels
			));
			glyphRects[glyphIndex].x = xOffset;
			glyphRects[glyphIndex].y = yOffset;
			glyphRects[glyphIndex].z = glyphSurface->w;
			glyphRects[glyphIndex].w = glyphSurface->h;
			SDL_FreeSurface(glyphSurface);
			glyphSurface = nullptr;
			xOffset += glyphRects[glyphIndex].z + padding;
		}
		yOffset += m_height + padding;
	}
	// Draw the unsupported glyph
	int pixelDims = padding - 1;
	int* pureWhiteSquare = new int[pixelDims * pixelDims];
	memset(pureWhiteSquare, 0xffffffff, pixelDims * pixelDims * sizeof(int));
	TRY_GL(glTexSubImage2D(
		GL_TEXTURE_2D, 0, 0, 0, pixelDims, pixelDims, GL_RGBA, GL_UNSIGNED_BYTE, pureWhiteSquare
	));
	delete[] pureWhiteSquare;
	pureWhiteSquare = nullptr;
	// Set some texture parameters
	TRY_GL((GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	TRY_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	TRY_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	TRY_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	// Create spriteBatch glyphs
	m_GlyphsPtr = new m_Glyph[m_asciiRange + 1];
	for (i = 0; i < m_asciiRange; i++) {
		m_GlyphsPtr[i].character = (char)(firstASCIIChar + i);
		m_GlyphsPtr[i].dimentions = glm::vec2(glyphRects[i].z, glyphRects[i].w);
		m_GlyphsPtr[i].uvRectangle = glm::vec4(
			(float)glyphRects[i].x / (float)bestWidth,
			(float)glyphRects[i].y / (float)bestHeight,
			(float)glyphRects[i].z / (float)bestWidth,
			(float)glyphRects[i].w / (float)bestHeight
		);
	}
	m_GlyphsPtr[m_asciiRange].character = ' ';
	m_GlyphsPtr[m_asciiRange].dimentions = m_GlyphsPtr[0].dimentions;
	m_GlyphsPtr[m_asciiRange].uvRectangle = glm::vec4(
		0,
		0,
		(float)pixelDims / (float)bestWidth,
		(float)pixelDims / (float)bestHeight
	);
	TRY_GL(glBindTexture(GL_TEXTURE_2D, 0));
	delete[] glyphRects;
	delete[] bestPartitions;
	TTF_CloseFont(fontPtr);
}

glm::vec2 TTFont::measure(const char* s) {
	glm::vec2 size(0, m_height);
	float cw = 0;
	for (auto si = 0; s[si] != 0; si++) {
		char c = s[si];
		if (s[si] == '\n') {
			size.y += m_height;
			if (size.x < cw) { size.x = cw; }
			cw = 0;
		} else {
			// Check for correct glyph
			int gi = c - m_asciiStart;
			if (gi < 0 || gi >= m_asciiRange) { gi = m_asciiRange; }
			cw += m_GlyphsPtr[gi].dimentions.x;
		}
	}
	if (size.x < cw) { size.x = cw; }
	return size;
}

void TTFont::generateCharSprites(const char* cString, SpriteBatch& spriteBatch, glm::vec2 scaling) {
	glm::vec2 tp(0);
	glm::vec2 m = measure(cString);
	tp.y += m.y * scaling.y - m_height * scaling.y;
	for (auto si = 0; cString[si] != 0; si++) {
		char c = cString[si];
		if (cString[si] == '\n') {
			tp.y -= m_height * scaling.y;
			tp.x = 0;
		} else {
			// Check for correct glyph
			int gi = c - m_asciiStart;
			if (gi < 0 || gi >= m_asciiRange) { gi = m_asciiRange; }
			fk::Texture t;
			t.id = m_textureID;
			int id = spriteBatch.makeSprite(t);
			spriteBatch[id].setPosition(tp);
			spriteBatch[id].setDimensions(
				m_GlyphsPtr[gi].dimentions.x * scaling.x,
				m_GlyphsPtr[gi].dimentions.y * scaling.y
			);
			tp.x += m_GlyphsPtr[gi].dimentions.x * scaling.x;
		}
	}
}

}