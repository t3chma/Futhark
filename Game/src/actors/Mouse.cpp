#include "Mouse.h"

Mouse::Mouse(fk::SpriteBatch& sb, fk::World& world, Mouse::Def& md) : Body(world, md.bd), Image(sb) {

	addCircleLimb(0.02).b2Ptr->SetSensor(true);
	p_sprites.emplace_back(p_spriteBatch, md.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(0.05, 0.05);
}

void Mouse::click(bool left) {

}

void Mouse::draw() {
	auto position = b2Ptr->GetPosition();
	p_sprites.front().setPosition(position.x, position.y);
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