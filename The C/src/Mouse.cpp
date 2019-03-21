#include "Mouse.h"



Mouse::Mouse(fk::SpriteBatch& sb, fk::Texture& texture, fk::World& world)
	: Object(world, b2_kinematicBody)
{
	b2Ptr->SetSleepingAllowed(false);
	sprites.emplace_back(sb, texture);
	sprites.back().setDimensions(0.1, 0.1);
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	addCircleLimb(0.1, 0, 0, &fixtureDef);
	category = "mouse";
}

void Mouse::updateBody() {
	b2Ptr->SetTransform(b2Vec2(position.x, position.y), 0);
}

void Mouse::updateSprites() {
	sprites.front().setPosition(position.x, position.y);
}

void Mouse::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	Object* object = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
	if (!collisionFixturePtr->IsSensor() || object->category == "order") {
		objectPtr = object;
	}
}

void Mouse::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	Object* endObject = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
	if (endObject == objectPtr) { objectPtr = nullptr; }
}
