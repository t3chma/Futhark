#include "Mouse.h"

Mouse::Mouse(fk::SpriteBatch& sb, fk::World& world, Mouse::Def& md) : Body(world, md.bd), Image(sb) {
	type = (int)Type::PLAYER;
	team = (int)Team::PLAYER;
	species = (int)Spec::MOUSE;
	addCircleLimb(0.02).b2Ptr->SetSensor(true);
	sprites.emplace_back(p_spriteBatch, md.body);
	sprites.back().setColor(255, 255, 255, 255); // white
	sprites.back().setDimensions(0.05, 0.05);
}

void Mouse::click(bool left) {
}

void Mouse::draw() {
	auto position = b2Ptr->GetPosition();
	sprites.front().setPosition(position.x, position.y);
}

void Mouse::think(fk::UserInput& ui) {
	b2Ptr->SetAwake(true);
	if (contacts.size()) {
		for (auto&& contact : contacts) {
			// Grab first contact
			Body& body = *static_cast<Body*>(contact->GetBody()->GetUserData());
			goal.location = glm::vec2(body.b2Ptr->GetPosition().x, body.b2Ptr->GetPosition().y);
			goal.bodyPtr = &body;
			break;
		}
	} else {
		goal.location = glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
		goal.bodyPtr = this;
	}
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
		contacts.insert(collisionFixturePtr);
	} else {

	}
}

void Mouse::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	contacts.erase(collisionFixturePtr);
}

bool Mouse::connect(Boid* bPtr) {
	return glm::length(
		glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y) - bPtr->goal
	) < 0.1;
}

bool Mouse::interact(Boid* bPtr) {
	auto push = bPtr->goal - glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y);
	glm::vec2 gush = glm::vec2(10) * glm::normalize(push);
	b2Vec2 bush = b2Vec2(gush.x, gush.y);
	bPtr->b2Ptr->ApplyForceToCenter(bush, true);
	return glm::length(push) < 0.1;
}