#pragma once
#include "../actors/Projectile.h"


class Equipment : public Image, public Intellect {
  public:
	struct Def {
		fk::Texture body;
	};
	Equipment(fk::SpriteBatch& sb, Equipment::Def ed);
	~Equipment();
	virtual Projectile* prime(int option) = 0;
	virtual Projectile* release(int option) = 0;
	virtual void draw() override;
	virtual void update(fk::UserInput& ui) override;
	std::list<Projectile> projectiles;
	Projectile::Def ProjectileSpawner;
};

