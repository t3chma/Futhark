#pragma once
#include "out/TTFont.h"
#include "../Image.h"
#include "../Body.h"
#include "../Intellect.h"
#include "in/FileCache.h"

class TextBlock : public Image, public Body, public Intellect {
  public:
	fk::Random r;
	std::list<fk::TextSprite> texts;
	std::list<Body*> reactors;
	TextBlock(char c, fk::TTFont& f, fk::SpriteBatch& sb, fk::World& w, Body::Def& bd, fk::Texture& t);
	~TextBlock();
	void setChar(char c);
	void draw() override;
	void update(fk::UserInput& ui) override;
	virtual void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	virtual void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
};

