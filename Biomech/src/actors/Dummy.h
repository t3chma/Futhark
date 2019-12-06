#pragma once
#include "Boid.h"

class Dummy : public Boid {
  public:
	struct Def : public Body::Def {
		fk::Texture body;
	};
	Dummy(fk::SpriteBatch& sb, fk::World& world, Dummy::Def& dd);
	~Dummy();
	virtual void p_think(fk::UserInput & ui) override;
	virtual void draw() override;
	virtual bool connect(Boid* bPtr) override;
	virtual bool interact(Boid* bPtr) override;
};

class DumRunner : public Dummy {
  public:
	DumRunner(fk::SpriteBatch& sb, fk::World& world, Dummy::Def& dd);;
	virtual void p_think(fk::UserInput& ui) override;
};