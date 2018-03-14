#pragma once
#include <string>
#include <vector>
#include "../in/FileCache.h"
#include "OutUtility.h"
namespace fk {


/* Handles drawing sprites to the screen.
[t3chma] */
class SpriteBatch {
  public:
	friend class SpriteRenderer;
	/* Information for drawing a sprite canvases to the screen.
	[t3chma] */
	struct Canvas {
		// Center position of this sprite. 12
		Vec3 position{ 0, 0, 0 };
		// Dimensions of the sprite halved. 20
		Vec2 dimensions{ 1, 1 };
		// UV coords of the bottom left corner. 28
		Vec2 texturePosition{ 0, 0 };
		// UV coords of the top right corner. 36
		Vec2 textureDimensions{ 1, 1 };
		// Axis of rotation for this sprite. 44
		Vec2 rotationAxis{ 0, 0 };
		// Amount of rotation around the rotation axis. 48
		GLfloat rotationAngle{ 0.0 };
		// Hue of the canvas. 52
		Color color{ 255, 255, 255, 255 };
	};
	struct Sprite {
		// Canvas. 52
		Canvas canvas;
		// Texture. 68
		Texture texture;
		void move(const glm::vec2& translation);
		void move(const float x, const float y);
		glm::vec2 getPosition() const;
		void setPosition(const glm::vec2& position);
		void setPosition(const float x, const float y);
		float getDepth() const;
		void setDepth(const float depth);
		void setDimensions(const float width, const float height);
		void setRotationAxis(const float x, const float y);
		void setColor(const char r, const char g, const char b, const char a);
		void setTexturePosition(const float x, const float y);
		void setTextureDimensions(const float width, const float height);
		void setFrame(const int frame);
		/* Swaps the texture for this sprite.
		(texture) The texture to associate to the sprite.
		(frames) How many animation frames this sprite's texture has.
		[t3chma] */
		void setTexture(const Texture& texture);
		Texture getTexture() const;
		/* Transforms this sprite into a line.
		The top of the texture will be at the A end and bottom of the texture at the B end.
		(b) The position of the B end.
		(a) The position of the A end.
		(thickness) The thickness of the line.
		[t3chma] */
		void makeLine(glm::vec2& b, glm::vec2& a, float thickness);
		void resetCanvas();
	};
	// Used to store the sorting info for the vertex buffer.
	struct SpriteTray {
		// Offset into the sprite buffer.
		int offset{ 0 };
		// How many sprites are in this tray.
		int size{ 1 };
		// The texture ID for this tray.
		GLuint textureID{ 0 };
		SpriteTray(GLuint textureID, int offset = 0);
	};
	/* Creates vertex array object.
	[t3chma] */
	SpriteBatch();
	/* Adds a sprite to the sprite batch.
	(texture) The texture to associate to the sprite.
	< The new sprite's ID.
	[t3chma] */
	int makeSprite(const Texture& texture);
	// Allows the retrieval of a sprite using its ID.
	Sprite& operator [] (int spriteID);
	/* Removes a sprite from the sprite batch.
	(spriteID) The ID of the sprite to kill.
	[t3chma] */
	void destroySprite(int spriteID);
  private:
	void m_makeSpriteTrays();
	/* Start the render pipeline.
	You probably should not even be calling this unless you are the renderer class.
	[t3chma] */
	void m_render();
	// Vertex buffer ID.
	GLuint m_vertexBufferObjectID;
	// Array buffer ID.
	GLuint m_vertexArrayObjectID;
	// Stores all sprites contiguously.
	std::vector<Sprite> m_spriteBuffer;
	// Any dead indices in the buffer.
	std::vector<int> m_deadBufferIndices;
	// Stores sprite pointers
	std::vector<Sprite*> m_spritePtrs;
	// Stores sorting info for m_processedSpritePtrs.
	std::vector<SpriteTray> m_spriteTrays;
	// The local vertex buffer to be sent to the GPU.
	std::vector<Canvas> m_vertexBuffer;
};

}