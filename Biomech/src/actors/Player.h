#pragma once
#include "Boid.h"
#include "Bot.h"
#include "Mouse.h"
#include "out/TTFont.h"
#include "out/Camera.h"


class Player : public Boid{
  public:
	struct Def : public Body::Def {
		fk::Texture body;
		Mouse::Def md;
		Bot::Def botSpawner;
	};
	Player(fk::SpriteBatch& sb, fk::SpriteBatch& tb, fk::TTFont& hudFont, fk::Camera& c, fk::World& w, Player::Def& pd);
	~Player();
	virtual void primeGadget(int option);
	virtual void primeGun(int option);
	virtual void p_think(fk::UserInput& ui) override;
	virtual void draw() override;
	virtual bool connect(Boid* bPtr) override;
	virtual bool interact(Boid* bPtr) override;
	Mouse mouse;
	fk::Camera& camera;
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