#pragma once
#include "Boid.h"
#include <set>


class Mouse : public Body, public Image, public Intellect {
public:
	struct Def {
		Body::Def bd;
		fk::Texture body;
	};
	Mouse(fk::SpriteBatch& sb, fk::World& world, Mouse::Def& md);
	void click(bool left);
	virtual void draw() override;
	virtual void think(fk::UserInput& ui) override;
	~Mouse();
	virtual void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	virtual void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	virtual bool connect(Boid* bPtr) override;
	virtual bool interact(Boid* bPtr) override;
	std::set<b2Fixture*> contacts;
	Goal goal;
};