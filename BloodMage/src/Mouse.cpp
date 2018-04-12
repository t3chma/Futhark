#include "Mouse.h"



Mouse::Mouse(Map& map, std::vector<fk::Texture>& textures)
	: Object(map.logicSprites, map.world, b2_kinematicBody)
{
	b2BodyPtr->SetSleepingAllowed(false);
	spriteIDs.reserve(textures.size());
	for (auto&& texture : textures) {
		spriteIDs.push_back(spriteBatch.makeSprite(texture));
		spriteBatch[spriteIDs.back()].setDimensions(1.0, 1.0);
	}
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
		spriteBatch[spriteIDs[0]].setPosition(orderEditPtr->getPosition());
	} else if (acotrEditPtr) {
		spriteBatch[spriteIDs[0]].setPosition(acotrEditPtr->getPosition());
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
