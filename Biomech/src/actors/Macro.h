#pragma once
#include "Boid.h"


class Macro : public Boid {
  public:
	Macro(fk::SpriteBatch& sb, fk::World& world);
	~Macro();
	void update(fk::UserInput& ui) override;
	void draw() override;
};

