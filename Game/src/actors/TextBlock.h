#pragma once
#include "out/TTFont.h"
#include "../Image.h"
#include "../Body.h"
#include "../Intellect.h"
#include "in/FileCache.h"

class TextBlock;
class Player;

class TextBlock : public Image, public Body, public Intellect {
  public:
	fk::Random rangen;
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
	void hitBlock(TextBlock* block, b2Fixture* myFixturePtr);
	void hitBullet(Body* bod, b2Fixture* myFixturePtr);
	void hitPlayer(Player* pod, glm::vec2 &u);
	virtual void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
};

