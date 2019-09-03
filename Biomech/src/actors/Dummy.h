#pragma once
#include "Boid.h"

class Dummy : public Boid {
  public:
	struct Def {
		Boid::Def bd;
		fk::Texture body;
	};
	Dummy(fk::SpriteBatch& sb, fk::World& world, Dummy::Def& dd);
	~Dummy();
	virtual void update(fk::UserInput & ui) override;
	virtual void draw() override;
	virtual bool connect(Boid* bPtr) override;
	virtual bool interact(Boid* bPtr) override;
};

class DumRunner : public Dummy {
  public:
	DumRunner(fk::SpriteBatch& sb, fk::World& world, Dummy::Def& dd) : Dummy(sb, world, dd) {
		pw.target = 0;
		pw.seek = 0;
		pw.snap = 0;
		p_speed = 10;
	};
	virtual void update(fk::UserInput & ui) override;
};