#pragma once
#include "Actor.h"


class Player : public Actor {
  public:
	Player(fk::SpriteBatch& sb, fk::World& world);
	~Player();
	void update(fk::UserInput& ui);
	void draw();
	fk::Key up{ fk::Key::W };
	fk::Key left{ fk::Key::A };
	fk::Key down{ fk::Key::S };
	fk::Key right{ fk::Key::D };
	fk::Key run{ fk::Key::SHIFT_L };
};