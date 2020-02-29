#pragma once
#include "../Body.h"
#include "../Image.h"
#include "../Intellect.h"
#include <set>


class Mouse : public Body, public Image, public Intellect {
public:
	struct Def {
		Body::Def bd;
		fk::Texture body;
	};
	Mouse(fk::SpriteBatch& sb, fk::World& world, Mouse::Def& md);
	void click(bool left);
	virtual void update(fk::UserInput& ui) override {};
	virtual void draw() override;
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
	std::set<b2Fixture*> contacts;
};