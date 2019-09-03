#pragma once
#include "Boid.h"
#include "Bot.h"
#include "Mouse.h"
#include "out/TTFont.h"


class Player : public Body, public Image, public Intellect {
  public:
	struct Def {
		Body::Def bd;
		Mouse::Def md;
		Bot::Def botSpawner;
		fk::Texture body;
		fk::TTFont hudFont;
	};
	Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd);
	~Player();
	virtual void update(fk::UserInput& ui) override;
	virtual void draw() override;
	virtual bool connect(Boid* bPtr) override;
	virtual bool interact(Boid* bPtr) override;
	Mouse mouse;
	fk::Key up{ fk::Key::W };
	fk::Key left{ fk::Key::A };
	fk::Key down{ fk::Key::S };
	fk::Key right{ fk::Key::D };
	HiveMind hive;
	std::vector<fk::TextSprite> tempHUD;
	fk::TTFont hudFont;
	Bot::Def botSpawner;
	bool aiming{ false };
};