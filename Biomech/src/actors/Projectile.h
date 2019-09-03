#pragma once
#include "../Body.h"
#include "../Image.h"
#include "../Intellect.h"


class Projectile : public Body, public Image, public Intellect {
  public:
	struct Def {
		Body::Def bd;
		fk::Texture body;
	};
	Projectile(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd);
	virtual void draw() override;
	virtual void update(fk::UserInput& ui) override;
	~Projectile();
};

