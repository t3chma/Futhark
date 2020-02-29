#pragma once
#include "out/SpriteBatch.h"
#include <list>

class Image {
  public:
	Image(fk::SpriteBatch& sb) : p_spriteBatch(sb) {};
	virtual void draw() = 0;
	void addSprite(const fk::Texture& T) { sprites.emplace_back(p_spriteBatch, T); }
	std::list<fk::Sprite> sprites;
	Image& operator = (const Image& rhs) {
		this->p_spriteBatch = rhs.p_spriteBatch; this->sprites = rhs.sprites; return *this;
	}
  protected:
	fk::SpriteBatch& p_spriteBatch;
};

