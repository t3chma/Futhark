#pragma once
#include <GLEW/glew.h>
/* Contains an ID and dimensions
[t3chma] */
namespace fk {


struct Texture {
	Texture() = default;
	Texture(GLuint id) : id(id) {};
	// GL texture ID.
	GLuint id{ 0 };
	// Resolution of texture.
	int width{ 0 }, height{ 0 };
	// Number of animation frames.
	int frames{ 1 };
	operator GLuint() { return id; };
};

}