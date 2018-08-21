#include "Mouse.h"



Mouse::Mouse(Map& map, std::vector<fk::Texture>& textures)
	: Object(map.logicSprites, map.world, b2_kinematicBody)
{
	b2BodyPtr->SetSleepingAllowed(false);
	sprites.add("cursor", textures[0]);
	sprites.get("cursor")->setDimensions(1.0, 1.0);
	b2FixtureDef fixtureDef1;
	b2CircleShape shape1;
	fixtureDef1.isSensor = true;
	fixtureDef1.shape = &shape1;
	shape1.m_radius = 0.1;
	fixtureDef1.userData = nullptr;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	category = "mouse";
}

void Mouse::updateBody() {
	b2BodyPtr->SetTransform(b2Vec2(position.x, position.y), 0);
}

void Mouse::updateSprite() {
	if (orderEditPtr) {
		sprites.get("cursor")->setPosition(orderEditPtr->getPosition());
	} else if (acotrEditPtr) {
		sprites.get("cursor")->setPosition(acotrEditPtr->getPosition());
	}
}

void Mouse::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Object* object = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
	if (!collisionFixturePtr->IsSensor() || object->category == "order") {
		objectPtr = object;
	}
}

void Mouse::p_endCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Object* endObject = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
	if (endObject == objectPtr) { objectPtr = nullptr; }
}
