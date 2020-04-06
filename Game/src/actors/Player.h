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
		fk::Texture shield;
		fk::TTFont* hudFont;
	};
	Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd);
	~Player();
	virtual void update(fk::UserInput& ui) override;
	virtual void draw() override;
	void setTeam(int t);
	Mouse mouse;
	struct Joys {
		fk::Joy xMove{ fk::Joy::LX };
		fk::Joy yMove{ fk::Joy::LY };
		fk::Joy xFire{ fk::Joy::RX };
		fk::Joy yFire{ fk::Joy::RY };
		fk::Joy fire{ fk::Joy::RZ };
		fk::Joy shield{ fk::Joy::LZ };
		fk::Joy interact{ fk::Joy::A };
	} joys;
	fk::Vec2 aim;
	long prevHealth{ 0 };
	Gun* gunPtr{ nullptr };
	bool freezeCam{ false };
	long immobilized{ 0 };
	long stunned{ 0 };
	long shield{ 0 };
	int reflectime{ 5 };
	int recharge{ 20 };
	long reflect{ recharge };
	long shealth{ 10 };
	bool interact{ false };
};