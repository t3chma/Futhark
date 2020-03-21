#pragma once
#include "Mouse.h"
#include "out/TTFont.h"
#include "Gun.h"

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
	void setTeam(int t) {
		team = t;
		gun.team = team;
	}
	Mouse mouse;
	struct Joys {
		fk::Joy xMove{ fk::Joy::LX };
		fk::Joy yMove{ fk::Joy::LY };
		fk::Joy xFire{ fk::Joy::RX };
		fk::Joy yFire{ fk::Joy::RY };
		fk::Joy fire{ fk::Joy::RZ };
	} joys;
	long prevHealth{ 0 };
	Gun gun;
	bool t{ false };
	bool freeze{ false };
	bool crank{ false };
};