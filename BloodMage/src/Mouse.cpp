#include "Mouse.h"



Mouse::Mouse(Map& map, fk::Texture& texture)
	: Object(map, b2_kinematicBody)
{
	b2Ptr->SetSleepingAllowed(false);
	sprites.emplace_back(map.logicSprites, texture);
	sprites.back().setDimensions(1.0, 1.0);
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	addCircleLimb(0.1, 0, 0, &fixtureDef);
	category = "mouse";
}

void Mouse::updateBody() {
	b2Ptr->SetTransform(b2Vec2(position.x, position.y), 0);
}

void Mouse::updateSprite() {
	if (orderEditPtr) {
		sprites.back().setPosition(orderEditPtr->getPosition().x, orderEditPtr->getPosition().y);
	} else if (acotrEditPtr) {
		sprites.back().setPosition(acotrEditPtr->getPosition().x, acotrEditPtr->getPosition().y);
	}
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
