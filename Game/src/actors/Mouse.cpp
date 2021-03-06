#include "Mouse.h"

Mouse::Mouse(fk::SpriteBatch& sb, fk::World& world, Mouse::Def& md) : Body(world, md.bd), Image(sb) {
	type = -1;
	addCircleLimb(0.02).b2Ptr->SetSensor(true);
	sprites.emplace_back(p_spriteBatch, md.body);
	sprites.back().setColor(255, 255, 255, 255); // white
	sprites.back().setDimensions(0.05, 0.05);
}

void Mouse::click(bool left) {

}

void Mouse::setColor(char r, char g, char b, char a) {
	sprites.back().setColor(r, g, b, a);
}

void Mouse::draw() {
	auto position = b2Ptr->GetPosition();
	sprites.front().setPosition(position.x, position.y);
}

Mouse::~Mouse() {

}

void Mouse::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (
		collisionFixturePtr != myFixturePtr &&
		!collisionFixturePtr->IsSensor() &&
		static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s"
	) {
		contacts.insert(collisionFixturePtr); }
}

void Mouse::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	contacts.erase(collisionFixturePtr);
}