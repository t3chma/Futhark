#pragma once
#include <string>
#include <vector>
#include <map>
#include "OutUtility.h"
#include "Texture.h"
namespace fk {


/* Handles drawing sprites to the screen.
[t3chma] */
class SpriteBatch {
  public:
	friend class SpriteRenderer;
	// Information for drawing a sprite canvases to the screen.
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
	// 2D Sprite.
	struct Sprite {
		// Canvas. 52
		Canvas canvas;
		// Texture. 68
		Texture texture;
		void move(const Vec2 translation);
		void move(const float x, const float y);
		Vec2 getPosition() const;
		void setPosition(const float x, const float y);
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
		/* Transforms this sprite into a line.
		The top of the texture will be at the A end and bottom of the texture at the B end.
		(b) The position of the B end.
		(a) The position of the A end.
		(thickness) The thickness of the line.
		[t3chma] */
		void makeLine(Vec2 b, Vec2 a, float thickness);
	};
	// Used to store the sorting info for the vertex buffer.
	struct SpriteTray {
		// Offset into the sprite buffer.
		int offset{ 0 };
		// How many sprites are in this tray.
		int size{ 1 };
		// The texture ID for this tray.
		GLuint textureID{ 0 };
		// The depth of this sprite batch.
		float depth{ 0 };
		SpriteTray(GLuint textureID, float depth = 0, int offset = 0);
	};
	/* Creates vertex array object.
	(dynamic) If the spritebatch will be changed often.
	[t3chma] */
	SpriteBatch(bool dynamic);
	/* Adds a sprite to the sprite batch.
	(texture) The texture to associate to the sprite.
	< The new sprite's ID.
	[t3chma] */
	int makeSprite(const Texture& texture);
	/* Adds a sprite to the sprite batch by copying an existing one.
	(spriteID) The ID of the the sprite to copy.
	< The new sprite's ID.
	[t3chma] */
	int copySprite(const int& spriteID);
	/* Allows the retrieval of a sprite using its ID.
	[t3chma] */
	Sprite& operator [] (int spriteID);
	/* Removes a sprite from the sprite batch.
	(spriteID) The ID of the sprite to kill.
	[t3chma] */
	void destroySprite(int spriteID);
  private:
	/* Sort sprites and store the info in m_spriteTrays.
	[t3chma] */
	void m_makeSpriteTrays();
	/* Start the render pipeline.
	You probably should not even be calling this unless you are the renderer class.
	[t3chma] */
	void m_render();
	// If what is being drawn is static or dynamic.
	bool m_dynamic{ true };
	// Vertex buffer ID.
	GLuint m_vertexBufferObjectID{ 0 };
	// Array buffer ID.
	GLuint m_vertexArrayObjectID{ 0 };
	// Stores all sprites contiguously.
	std::vector<Sprite> m_spriteBuffer;
	// Any dead indices in the sprite buffer.
	std::vector<int> m_deadBufferIndices;
	// Stores sprite pointers
	std::vector<Sprite*> m_spritePtrs;
	// Stores sorting info for m_spritePtrs.
	std::vector<SpriteTray> m_spriteTrays;
	// The local vertex buffer to be sent to the GPU.
	std::vector<Canvas> m_vertexBuffer;
	// If the local static buffer needs to be sent to the GPU.
	bool m_bufferStatic{ false };
};

class Sprite {
  private:
	int m_id{ -1 };
	SpriteBatch* m_spriteBatchPtr{ nullptr };
  public:
	Sprite(SpriteBatch& spriteBatch, const Texture& texture);
	Sprite(const Sprite& sprite);
	~Sprite();
	SpriteBatch::Canvas& getCanvasRef();
	SpriteBatch& getSpriteBatchRef();
	int getID() const;
	void move(const Vec2 translation);
	void move(const float x, const float y);
	glm::vec2 getPosition() const;
	void setPosition(const Vec2 position);
	void setPosition(const float x, const float y);
	void setDimensions(const Vec2 position);
	void setDimensions(const float width, const float height);
	void setRotationAxis(const Vec2 position);
	void setRotationAxis(const float x, const float y);
	void setRotationAngle(const float r);
	void setTexturePosition(const Vec2 position);
	void setTexturePosition(const float x, const float y);
	void setTextureDimensions(const Vec2 position);
	void setTextureDimensions(const float width, const float height);
	void setFrame(const int frame);
	void setColor(const char r, const char g, const char b, const char a);
	/* Swaps the texture for this sprite.
	(texture) The texture to associate to the sprite.
	(frames) How many animation frames this sprite's texture has.
	[t3chma] */
	void setTexture(const Texture& texture);
	/* Transforms this sprite into a line.
	The top of the texture will be at the A end and bottom of the texture at the B end.
	(b) The position of the B end.
	(a) The position of the A end.
	(thickness) The thickness of the line.
	[t3chma] */
	void makeLine(Vec2 b, Vec2 a, float thickness);
};

}