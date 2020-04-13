#pragma once
#include "out/TTFont.h"
#include "../Image.h"
#include "../Intellect.h"
#include "../Body.h"
#include "in/FileCache.h"

class TextBlock;
class Player;

class Gun : public Intellect {
  private:
	class M_bullet : public Image, public Body, public Intellect {
	  public:
		bool rebound{ false };
		std::vector<Player*> reactors;
		bool fresh{ true };
		fk::Random r;
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
		void hitBlock(TextBlock* tod, fk::Vec2 &u);
		void hitEnemy(Player* pod, b2Contact* contactPtr, fk::Vec2 &u, b2Fixture* myFixturePtr);
	};
  public:
	fk::Random rangen;
	fk::SpriteBatch& sb;
	fk::TextSprite text;
	int team{ 0 };
	Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex, fk::TextSprite& f);
	void fire(Body* ownerPtr, fk::Vec2 spawn, fk::Vec2 direction, int level);
	void setUpgrade(char u);
	virtual void update(fk::UserInput& ui) override;
	~Gun();
	fk::World& w;
	fk::Texture bulTex;
	std::list<M_bullet> bullets;
	long charge{ 0 };
	char upgrade{ '`' };
	long lastFire{ 0 };
	long clipSize{ 7 };
	long ammo{ clipSize };
	long reloadTime{ 60 };
	long reload{ 0 };
	bool trigger{ false };
};