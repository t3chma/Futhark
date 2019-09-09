#pragma once
#include "../Body.h"
#include "../Image.h"
#include "../Intellect.h"
class Equipment;


class Projectile : public Body, public Image, public Intellect {
  public:
	struct Def {
		Body::Def bd;
		fk::Texture body;
		float size{ 0.1 };
		int lifetime{ static_cast<int>(fk::SECOND) * 5 };
		float launchSpeed{ 50 };
		Equipment* makerPtr{ nullptr };
		/// other gun stuff here
	};
	Projectile(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void draw() override;
	virtual void think(fk::UserInput& ui) override {};
	~Projectile();
	int lifetime;
	Equipment* makerPtr{ nullptr };
};

class SField : public Projectile {
public:
	SField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	);
};

class HField : public Projectile {
  public:
	HField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	);
};

class CPField : public Projectile {
  public:
	  CPField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	  virtual void p_preCollisionAdjusting(
		  b2Fixture* collisionFixturePtr,
		  b2Fixture* myFixturePtr,
		  b2Contact* contactPtr,
		  const b2Manifold* oldManifoldPtr
	  );
};

class DDust : public Projectile {
  public:
	DDust(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	);
	virtual void think(fk::UserInput& ui) override;
};

class Chain : public Projectile {
  public:
	Chain(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	void think(fk::UserInput & ui);
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	);
};