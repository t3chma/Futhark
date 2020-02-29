#pragma once
#include "out/SpriteBatch.h"
#include <list>

class Image {
  public:
	Image(fk::SpriteBatch& sb) : p_spriteBatch(sb) {};
	virtual void draw() = 0;
  protected:
	fk::SpriteBatch& p_spriteBatch;
	std::list<fk::Sprite> p_sprites;
};

