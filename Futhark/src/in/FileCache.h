#pragma once
#include <map>
#include "../out/TTFont.h"
#include "../out/Shader.h"
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
		// Look for data
		auto mapSelection = m_map.find(filePath);
		if (mapSelection != m_map.end()) {
			// If data is found return it
			return mapSelection->second;
		} else {
			// If data is not found then load a new data and put the data into the cache
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


/* Loads and stores PNG files as textures in memory.
[t3chma] */
class TextureCache : public FileCache<Texture> {
  public:
	/* Get a texture from the given file path and assign its frame count.
	If the texture is not already in memory it loads it in.
	(filepath) The file path to the texture.
	(frames) The number of animation frames for this texture.
	< The texture associated to the file path.
	[t3chma] */
	Texture get(const std::string& filePath, int frames = 1);
  protected:
	/* Load texture from the given PNG file path.
	(filepath) The file path to the texture.
	< The texture.
	[t3chma] */
	Texture p_load(const std::string& filePath) override;
};


/* Loads and stores TTF files as fonts in memory.
[t3chma] */
class FontCache : public FileCache<TTFont> {
protected:
	/* Load font from the given TTF file path.
	(filepath) The file path to the font.
	< The font.
	[t3chma] */
	TTFont p_load(const std::string& filePath) override;
};


/* Loads and stores GLSLShaders in memory.
[t3chma] */
class ShadersCache : public FileCache<Shader> {
  protected:
	/* Load shader from the given glsl file path.
	(filepath) The file path to the shader.
	< The shader.
	[t3chma] */
	Shader p_load(const std::string& filePath) override;
};

/* Loads and stores DTM files in memory.
[t3chma] */

//class TreeCache : public FileCache<DTree> {
//protected:
//	/* Load decision tree from the given DTM file path.
//	(filepath) The file path to the decision tree.
//	< The decision tree.
//	[t3chma] */
//	DTree p_load(const std::string& filePath) override;
//};

}