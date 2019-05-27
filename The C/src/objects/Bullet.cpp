#include "Bullet.h"


Bullet::Bullet(
	fk::World& world,
	glm::vec2 position,
	glm::vec2 velocity,
	float size
) : Object(
		world,
		b2_dynamicBody,
		position.x,
		position.y,
		10,
		10,
		0,
		false,
		true
	)
{
	addCircleLimb(size);
	b2Ptr->ApplyForceToCenter(b2Vec2(velocity.x, velocity.y), true);
}

void Bullet::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	hitFixturePtrs.insert(collisionFixturePtr);
}

void Bullet::p_endCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	//--collisions;
	//hitFixturePtrs.erase(collisionFixturePtr);
}
