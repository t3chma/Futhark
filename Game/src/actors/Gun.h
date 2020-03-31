#pragma once
#include "out/TTFont.h"
#include "../Image.h"
#include "../Intellect.h"
#include "../Body.h"
#include "in/FileCache.h"
#include "out/TTFont.h"

class Gun : public Intellect {
  private:
	class M_bullet : public Image, public Body, public Intellect {
	  public:
		char upgrade{ '`' };
		int bounces{ 0 };
		glm::vec2 oldPos{ 0,0 };
		M_bullet(Body* ownerPtr, fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t, int level = 0);
		virtual void draw() override;
		virtual void update(fk::UserInput& ui) override;
		virtual void p_beginCollision(
			b2Fixture* collisionFixturePtr,
			b2Fixture* myFixturePtr,
			b2Contact* contactPtr
		) override;
	};
  public:
	fk::SpriteBatch& sb;
	fk::TextSprite text;
	int team{ 0 };
	Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex, fk::TextSprite& f);
	void fire(Body* ownerPtr, fk::Vec2 spawn, fk::Vec2 direction, int level);
	virtual void update(fk::UserInput& ui) override;
	~Gun();
	fk::World& w;
	fk::Texture bulTex;
	std::list<M_bullet> bullets;
	long charge{ 0 };
	char upgrade{ '`' };
	long lastFire{ 0 };
};

