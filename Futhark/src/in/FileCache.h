#pragma once
#include <map>
#include <GLEW\glew.h>
namespace fk {


/* Loads and stores file data in memory.
[t3chma] */
template <class T>
class FileCache {
  public:
	FileCache() = default;
	/* Get data (T) from the given file path.
	If the data (T) is not already in memory it loads it in.
	(filepath) The file path to the data.
	< The data (T) associated to the file path.
	[t3chma] */
	virtual T get(const std::string& filePath) final {
		// Look for texture
		auto mapSelection = m_map.find(filePath);
		if (mapSelection != m_map.end()) {
			// If texture is found return it
			return mapSelection->second;
		} else {
			// If texture is not found then load a new image and put the texture into the cache
			m_map[filePath] = p_load(filePath);
			return m_map[filePath];
		}
	};
  protected:
	/* How to load the data (T).
	(filePath) The file path to the data.
	< The actual data (T) at the file path.
	[t3chma] */
	virtual T p_load(const std::string& filePath) = 0;
  private:
	// A map of the loaded files.
	std::map<std::string, T> m_map;
};

/* Contains an ID and dimensions
[t3chma] */
struct Texture {
	// GL texture ID.
	GLuint id;
	// Resolution of texture.
	int width, height;
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
	Texture p_load(const std::string& filePath) override;
};


/* Handles the compilation, linking, and usage of a GLSL shader program.
^ http://www.opengl.org/wiki/Shader_Compilation
[t3chma] */
struct Shader {
	// The type of shader.
	enum Type { VERT, FRAG, GEOM, TESC, TESE, COMP };
	// GL shader ID.
	GLuint id{ 0 };
	// The type of shader.
	Type type;
};


/* Loads and stores GLSLShaders in memory.
[t3chma] */
class ShadersCache : public FileCache<Shader> {
  protected:
	/* Load texture from the given PNG file path.
	(filepath) The file path to the texture.
	< The texture.
	[t3chma] */
	Shader p_load(const std::string& filePath) override;
};

}