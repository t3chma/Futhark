#pragma once
#include "out/SpriteBatch.h"
#include <list>

class Image {
  public:
	Image(fk::SpriteBatch& sb) : p_spriteBatch(sb) {};
	virtual void draw() = 0;
	std::list<fk::Sprite> sprites;
  protected:
	fk::SpriteBatch& p_spriteBatch;
};

