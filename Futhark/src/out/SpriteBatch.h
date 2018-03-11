#pragma once
#include <string>
#include <vector>
#include "../in/FileCache.h"
#include "Utility.h"
namespace fk {


/* Handles drawing sprites to the screen.
[t3chma] */
class SpriteBatch {
  public:
	friend class SpriteRenderer;
	/* Information for drawing a sprite canvases to the screen.
	[t3chma] */
	struct Canvas {
		// Center position of this sprite.
		Vec3 position{ 0, 0, 0 };
		// Dimensions of the sprite halved.
		Vec2 dimensions{ 1, 1 };
		// UV coords of the bottom left corner.
		Vec2 texturePosition{ 0, 0 };
		// UV coords of the top right corner.
		Vec2 textureDimensions{ 1, 1 };
		// Axis of rotation for this sprite.
		Vec2 rotationAxis{ 0, 0 };
		// Amount of rotation around the rotation axis.
		GLfloat rotationAngle{ 0.0 };
		// Hue of the canvas.
		Color color{ 255, 255, 255, 255 };
	};
	/* A wrapper for an std::vector which allows for keep track of when it is modified.
	Stands for Texture Buffer because each buffer is associated to a texture.
	[t3chma] */
	class TBuffer : public std::vector<Canvas> {
	  public:
		// If this buffer has been modified
		bool modified{ false };
		// Used to keep track of where the sprite tray begins in memory.
		GLuint offset{ 0 };
	};
	/* Used as an interface to buffered Sprite info (for a single sprite).
	[t3chma] */
	class Sprite {
	  public:
		friend class SpriteBatch;
		Sprite() = delete;
		/* Used to manage a buffered sprite.
		Use a sprite batch to make these.
		(bufferPtr) A pointer to the buffer where this sprite's info is.
		(bufferIndex) The index into the buffer.
		(texturePtr) Pointer to the texture associated with this sprite.
		(batchPtr) A pointer to the SpriteBatch this belongs to.
		[t3chma] */
		Sprite(
			SpriteBatch::TBuffer* const bufferPtr,
			const int bufferIndex,
			const Texture& texture,
			SpriteBatch* const batchPtr
		);
		Canvas& getCanvasRef();
		const Canvas& getCanvasConstRef() const;
		Texture getTextureCopy() const;
		SpriteBatch* getBatchPtr() const;
		/* Checks if this sprite is alive.
		< If the sprite is alive.
		[t3chma] */
		bool isAlive() const;
		/* Kills this sprite.
		[t3chma] */
		void kill();
		/* Transforms this sprite into a line.
		The top of the texture will be at the A end and bottom of the texture at the B end.
		(b) The position of the B end.
		(a) The position of the A end.
		(thickness) The thickness of the line.
		[t3chma] */
		void makeLine(glm::vec2& b, glm::vec2& a, float thickness);
	  private:
		// Pointer to the sprite buffer.
		SpriteBatch::TBuffer* m_bufferPtr{ nullptr };
		// Index into the buffer.
		int m_bufferIndex{ -1 };
		// Copy of the texture associated with this sprite.
		Texture m_texture{ 0 };
		// Pointer to the batch this sprite belongs to.
		SpriteBatch* m_batchPtr{ nullptr };
	};
	/* Creates vertex array object.
	[t3chma] */
	SpriteBatch();
	/* Adds a sprite to the sprite batch.
	(texture) The texture to associate to the sprite.
	< The new sprite.
	[t3chma] */
	Sprite makeSprite(Texture& texture);
	/* Removes a sprite from the sprite batch.
	(sprite) The sprite to kill. DO NOT USE THIS AFTER PASSING IT IN!
	[t3chma] */
	void killSprite(Sprite& sprite);
	/* Mark the sprite batch as needing to be rebuffered before rendering again.
	[t3chma] */
	void flagRebuffer();
  private:
	/* Start the render pipeline.
	You probably should not even be calling this unless you are the renderer class.
	[t3chma] */
	void m_render();
	// Vertex buffer ID.
	GLuint m_vertexBufferObjectID;
	// Array buffer ID.
	GLuint m_vertexArrayObjectID;
	struct textureComparator {
		bool operator () (const Texture& lhs, const Texture& rhs) const { return lhs.id < rhs.id; }
	};
	// Map of texture buffers.
	std::map <Texture, TBuffer, textureComparator> m_bufferMap;
	// If the buffer map has changed;
	bool m_modified{ true };
	// If the buffer has been sent to the GPU this frame.
	bool m_buffered{ false };
	// The actual vertex buffer to send to openGL.
	std::vector<Canvas> m_vertexBuffer;
};

}