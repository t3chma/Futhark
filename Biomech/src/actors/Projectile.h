#pragma once
#include "../Body.h"
#include "../Image.h"
#include "../Intellect.h"
class Equipment;


class Projectile : public Body, public Image, public Intellect {
  public:
	struct Properties {
		// Pointer to the equipment that made this
		Equipment* makerPtr{ nullptr };
		// How many frames this bullet has to live
		int lifetime{ 60 };
		// Physical damage
		struct {
			float power{ 0 };
			int durration{ 1 };
		} physical;
		// Heat damage
		struct {
			float power{ 0 };
			int durration{ 1 };
		} heat;
		// Corruption damage
		struct {
			float power{ 0 };
			int durration{ 5 * 60 };
		} corruption;
		// Damage drop off
		struct {
			float start{ 1 };
			float reduction{ 0 };
		} dropoff;
		// Number of bounces/peirces
		int health{ 1 };
		// Energy cost
		int cost{ 1 };
		// size of bullet
		float size{ 0.1 };
		// Splash dropoff
		struct {
			float range{ 0 };
			float innerDamage{ 1 };
			float outerDamage{ 1 };
		} splash;
	} properties;
	struct Def : public Body::Def, public Properties {
		fk::Texture body;
	};
	Projectile(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void draw() override;
	virtual void think(fk::UserInput& ui) override {};
	~Projectile();
};

class SField : public Projectile {
public:
	bool triggered{ false };
	SField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void think(fk::UserInput& ui) override {};
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
	virtual void think(fk::UserInput& ui) override;
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	);
};

class Chain : public Projectile {
  public:
	Chain(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void think(fk::UserInput& ui) override;
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	);
};