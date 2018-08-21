#include "Thrown.h"


Thrown::Thrown(Map& map, PropDef pd, glm::vec2& direction) :
	Prop(map, pd)
{
	map.propPtrs.push_back(this);
	category = "thrown";
	sprites.add("body", pd.texture);
	sprites.get("body")->setDimensions(pd.size, pd.size);
	sprites.get("body")->setPosition(pd.position);
	sprites.add("blood", pd.texture);
	sprites.get("blood")->setColor(0, 0, 0, 0);
	sprites.get("blood")->setDimensions(pd.size, pd.size);
	sprites.get("blood")->setPosition(pd.position);
	b2CircleShape shape1;
	shape1.m_radius = pd.size / 2;
	b2FixtureDef fixtureDef1;
	fixtureDef1.shape = &shape1;
	fixtureDef1.userData = (void*)1;
	fixtureDef1.density = 10.0f;
	fixtureDef1.friction = 0.3f;
	fixtureDef1.filter.categoryBits = 4;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	b2BodyPtr->ApplyAngularImpulse(1, true);
	b2BodyPtr->ApplyLinearImpulse(
		b2Vec2(direction.x, direction.y),
		b2BodyPtr->GetWorldCenter(),
		true
	);
}
Thrown::~Thrown() {

}
void Thrown::update(fk::Camera* camPtr) {

}
void Thrown::updateBody() {
	if (m_hitPtr && b2BodyPtr->GetFixtureList()) {
		b2BodyPtr->DestroyFixture(b2BodyPtr->GetFixtureList());
	}
}
void Thrown::updateSprite() {
	Prop::updateSprite();
	if (m_hitPtr) {
		sprites.get("blood")->setPosition(m_hitPtr->getPosition());
	}
}
void Thrown::p_beginCollision(b2Fixture * collisionFixturePtr, b2Fixture * myFixturePtr, b2Contact * contactPtr) {
	if (!collisionFixturePtr->IsSensor() && myFixturePtr->GetUserData() != nullptr) {
		sprites.get("body")->setColor(0, 0, 0, 0);
		sprites.get("blood")->setColor(255, 255, 255, 0);
		m_hitPtr = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
	}
}
