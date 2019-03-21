#pragma once
#include "up/Body.h"
#include "out/SpriteBatch.h"
#include "in/UserInput.h"


class Actor : public fk::Body {
  public:
	Actor(fk::SpriteBatch& sb, fk::World& world);
	virtual void update(fk::UserInput& ui) = 0;
	virtual void draw() = 0;
  protected:
	long p_health{ 0 };
	int p_speed{ 0 };
	glm::vec2 p_direction{ 0 };
	fk::SpriteBatch& p_spriteBatch;
};