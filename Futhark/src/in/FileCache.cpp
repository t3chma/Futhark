#include "FileCache.h"
#include "IOManager.h"
#include "../base/Error.h"
#include "PicoPNG.h"
namespace fk {


template <class T>
T FileCache<T>::get(const std::string& IMAGE_FILE_PATH) {
	Texture texture{};

	// Look for texture
	auto mapSelection = m_map.find(IMAGE_FILE_PATH);
	if (mapSelection != m_map.end()) {
		// If texture is found return it
		texture = mapSelection->second;
	}
	else {
		// If texture is not found then load a new image

		// Then put the texture into the cache
		texture = load(IMAGE_FILE_PATH);
		m_map[IMAGE_FILE_PATH] = texture;
	}

	return texture;
}



Texture::Texture(GLuint id) : id(id) {}

Texture::operator GLuint() { return id; }

Texture TextureCache::load(const std::string& IMAGE_FILE_PATH) {
	Texture texture{};
	std::vector<unsigned char> binaryPNG;
	std::vector<unsigned char> decodedPNG;
	unsigned long width, height;

	// Load
	IOManager io;
	if (io.readBinaryFileToBuffer(IMAGE_FILE_PATH, binaryPNG) == false) {
		LOG_LINE("IOManager failed to load PNG: " + IMAGE_FILE_PATH);
	}

	// Decode
	int errorPICO = decodePNG(decodedPNG, width, height, &(binaryPNG[0]), binaryPNG.size());
	if (errorPICO != 0) { LOG_LINE("picoPNG failed to decode: " + IMAGE_FILE_PATH); }

	// Make GLtexture
	texture.width = width;
	texture.height = height;
	// Create a blank openGL texture
	// https://www.opengl.org/sdk/docs/man/html/glGenTextures.xhtml
	glGenTextures(1, &texture.id);
	// Open openGL texture
	// https://www.opengl.org/sdk/docs/man/html/glBindTexture.xhtml
	glBindTexture(GL_TEXTURE_2D, texture.id);
	// Specify a two-dimensional texture image
	// https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(decodedPNG[0]));
	// Set texture parameters
	// https://www.opengl.org/sdk/docs/man/html/glTexParameter.xhtml
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Generate mipmaps for a specified texture object
	// https://www.opengl.org/sdk/docs/man/html/glGenerateMipmap.xhtml
	glGenerateMipmap(GL_TEXTURE_2D);
	// Close openGL texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}



// "<default>" is a preset
//GLSLShaders GLSLShadersCache::get(const std::string& BASE_FILE_PATH) {
//	// Try to find the shaders in the cache
//	auto it = m_GLSLShadersCacheMap.find(BASE_FILE_PATH);

//	GLSLShaders shaders;

//	// If shaders are not found then load a new image
//	if (it == m_GLSLShadersCacheMap.end()) {
//		shaders.compile(BASE_FILE_PATH);
//		shaders.addAttribute("vertexPosition");
//		shaders.addAttribute("vertexColor");
//		shaders.addAttribute("vertexUV");
//		shaders.link();

//		m_GLSLShadersCacheMap[BASE_FILE_PATH] = shaders;
//	}
//	else {
//		shaders = it->second;
//	}

//	return shaders;
//}

}