#pragma once
#include "Mouse.h"
#include "out/TTFont.h"


class Player : public Body, public Image, public Intellect {
  public:
	struct Def {
		Body::Def bd;
		Mouse::Def md;
		fk::Texture body;
		fk::TTFont hudFont;
	};
	Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd);
	~Player();
	virtual void update(fk::UserInput& ui) override;
	virtual void draw() override;
	Mouse mouse;
	fk::Key up{ fk::Key::W };
	fk::Key left{ fk::Key::A };
	fk::Key down{ fk::Key::S };
	fk::Key right{ fk::Key::D };
};