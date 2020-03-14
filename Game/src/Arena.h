#pragma once
#include "actors/TextBlock.h"

class Arena {
  public:
	std::list<std::list<TextBlock>> map;
	Arena(std::string& levelPath, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w);
	~Arena();
	void draw();
	void update(fk::UserInput & ui);
};

