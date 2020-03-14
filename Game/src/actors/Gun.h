#pragma once
#include "out/TTFont.h"
#include "../Image.h"
#include "../Body.h"
#include "in/FileCache.h"

class Gun : public Image {
  private:
	class M_bullet : public Image, public Body {
	  public:
		int bounces{ 0 };
		M_bullet(fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t);
		virtual void draw() override;
		virtual void p_beginCollision(
			b2Fixture* collisionFixturePtr,
			b2Fixture* myFixturePtr,
			b2Contact* contactPtr
		) override;
	};
  public:
	int team{ 0 };
	Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex);
	void fire(fk::Vec2 spawn, fk::Vec2 target);;
	virtual void draw() override;;
	~Gun();
	fk::World& w;
	fk::Texture bulTex;
	std::list<M_bullet> bullets;
};

