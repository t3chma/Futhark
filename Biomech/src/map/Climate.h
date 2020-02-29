#pragma once
#include "../Stats.h"
#include <vector>
#include "../Image.h"
#include "base/Tools.h"

class Climate {
  public:
	class Biome : public StatEffects, Image {
	  public:
		enum Type { PLAIN = 0, TREES, DESERT, ICE, FLOWERS, RUINS, RIVER, GOOP } type;
		Biome(fk::TextureCache& t, fk::SpriteBatch& sb) : Image(sb), textures(t){
			sprites.emplace_back(sb, fk::Texture(0));
			sprites.back().getCanvasRef().position.z = 1;
		}
		fk::TextureCache& textures;
		void setType(Type t);
		void draw() override;
		Biome& operator = (const Biome& rhs) { setType(rhs.type); return *this; }
	} biome;
	class Ground : public StatEffects, Image {
	  public:
		enum Type { FLAT = 0, MINES, CRATER, HILL, MOUNTAIN, VOID } type;
		Ground(fk::TextureCache& t, fk::SpriteBatch& sb) : Image(sb), textures(t) {
			sprites.emplace_back(sb, fk::Texture(0));
			sprites.back().getCanvasRef().position.z = 1;
		}
		fk::TextureCache& textures;
		void setType(Type t);
		void draw() override;
		Ground& operator = (const Ground& rhs) { setType(rhs.type); return *this; }
	} ground;
};