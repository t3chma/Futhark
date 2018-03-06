#pragma once
#include <map>
#include <GLEW\glew.h>
//#include "GLSLShaders.h"
namespace fk {


/* Loads and stores file data in memory.
[t3chma] */
template <class T>
class FileCache {
public:

	Cache() = default;

	/* Get data (T) from the given file path.
	If the data (T) is not already in memory it loads it in.
	(filepath) The file path to the data.
	< The data (T) associated to the file path.
	[t3chma] */
	virtual T get(const std::string& filePath) final;

protected:

	/* How to load the data (T).
	(filePath) The file path to the data.
	< The actual data (T) at the file path.
	[t3chma] */
	virtual T load(const std::string& filePath) = 0;

private:

	// A map of the loaded files.
	std::map<std::string, T> m_map;
};



/* Contains an ID and dimensions
[t3chma] */
struct Texture {

	GLuint id;
	int width, height;

	Texture() = default;

	Texture(GLuint id);

	operator GLuint();

};



/* Loads and stores PNG files as textures in memory.
[t3chma] */
class TextureCache : public FileCache<Texture> {

protected:

	/* Load texture from the given PNG file path.
	(filepath) The file path to the texture.
	< The texture.
	[t3chma] */
	Texture load(const std::string& filePath) override;
};


		
/* Loads and stores FRAG and VERT files as GLSLShaders in memory.
IMPORTANT: This can only be used with shaders that use the default non-uniform variables:
position, color, and uv. You need to load more specialized shaders in yourself.
[t3chma] */
class GLShadersCache {

public:

	/* Get a GLSLShaders from the given FRAG and VERT base file paths.
	Both shaders must have the same name and they must have the ".vert" and ".frag" extensions.
	"<default>" is a preset and loads the default shaders which are hard coded.
	If the shaders are not already in memory it loads them into it.
	(filepath) The base file path to the shaders.
	< The texture.
	| // Gets the GLSLShaders which uses the myShader.frag and myShader.vert shaders.
	| ResourceCache::shaders.get("myShader");
	[t3chma] */
	//GLSLShaders get(const std::string& baseFilePath = "<default>");

private:

	// A map of all the loaded GLSLShaders.
	//std::map<std::string, GLSLShaders> m_GLSLShadersCacheMap;
};

}