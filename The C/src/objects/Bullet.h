#pragma once
#include "../actors/Actor.h"


class Bullet : public Object {
  public:
	Bullet(
		fk::World& world,
		glm::vec2 position,
		glm::vec2 velocity,
		float size
	);
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
	int collisions{ 0 };
	int lifetime{ 0 }
};