#pragma once
#include "Mouse.h"
#include "out/TTFont.h"
#include "Gun.h"

class TextBlock;

class Player : public Body, public Image, public Intellect, public b2RayCastCallback {
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
	virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction);
	virtual void update(fk::UserInput& ui) override;
	void GetAIInput();
	void getPlayerInput(fk::UserInput & ui);
	virtual void draw() override;
	void setTeam(int t);
	void p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr);
	void p_endCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr);
	b2Fixture* los{ false };
	Mouse mouse;
	bool ai{ false };
	struct Joys {
		fk::Joy xMove{ fk::Joy::LX };
		fk::Joy yMove{ fk::Joy::LY };
		fk::Joy xFire{ fk::Joy::RX };
		fk::Joy yFire{ fk::Joy::RY };
		fk::Joy fire{ fk::Joy::RZ };
		fk::Joy reload{ fk::Joy::R };
		fk::Joy shield{ fk::Joy::LZ };
		fk::Joy dodge{ fk::Joy::L };
		fk::Joy interact{ fk::Joy::A };
	} joys;
	fk::Vec2 aim;
	fk::Vec2 move;
	long prevHealth{ 0 };
	Gun* gunPtr{ nullptr };
	bool freezeCam{ false };
	long immobilized{ 0 };
	long stunned{ 0 };
	long shield{ 0 };
	int reflectime{ 5 };
	int recharge{ 60 };
	long reflect{ recharge };
	long shealth{ 10 };
	bool interact{ false };
	long dodge{ 0 };
	int dodgeFrames{ 35 };
	std::list<Body*> reactors;
	std::list<TextBlock*> blocks;
	std::list<Player*> players;
	fk::Random rangen;
	long shieldTimer{ 0 };
	long shieldBreakTime{ -60 };
};