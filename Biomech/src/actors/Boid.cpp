#include "Boid.h"


Boid::Boid(fk::SpriteBatch& sb, fk::World& world, float sightRadius)
	: Actor(sb, world), los(sightRadius)
{
	category = "boid";
	addCircleLimb(sightRadius).b2Ptr->SetSensor(true);
	limbs.back().category = "sight";
}
Boid::~Boid() {

}
void Boid::update(fk::UserInput& ui) {
	/// TODO: use contact list for all things not alignment.
	// Get my position.
	b2Vec2 pos = b2Ptr->GetPosition();
	glm::vec2 myPos(pos.x, pos.y);
	// Get alignment.
	float alignment{ 0 };
	for (auto&& angle : los.friendAngles) { alignment += angle; }
	alignment /= los.friendAngles.size();
	// Get cohesion and seperation.
	glm::vec2 cohesion{ 0, 0 };
	glm::vec2 seperation{ 0, 0 };
	for (auto&& fPos : los.friendPos) {
		cohesion += fPos;
		seperation += glm::vec2(los.radius) / glm::vec2(myPos - fPos);
	}
	cohesion /= los.friendPos.size();
	seperation /= los.friendPos.size();
	// Get avoidance
	glm::vec2 avoidance{ 0, 0 };
	for (auto&& bPos : los.badPos) { seperation += glm::vec2(los.radius) / glm::vec2(myPos - bPos); }
	avoidance /= los.badPos.size();
	// Get seek
	glm::vec2 seek = glm::normalize(goal - myPos);
	// Apply spin
	float mAngle = b2Ptr->GetAngle();
	b2Ptr->ApplyTorque(alignment - mAngle, true);
	glm::vec2 forward = glm::vec2(cos(mAngle) * p_speed, sin(mAngle) * p_speed);
	// Apply move
	glm::vec2 force{ forward + seek + cohesion + seperation + avoidance };
	b2Ptr->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
	// Clear los info
	los.badPos.clear();
	los.friendAngles.clear();
	los.friendPos.clear();
}
void Boid::draw() {

}
void Boid::p_postCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2ContactImpulse* impulsePtr
) {
	if (!collisionFixturePtr->IsSensor()) {
		b2Body& cBody = *collisionFixturePtr->GetBody();
		if (!collisionFixturePtr->IsSensor()) {
			b2WorldManifold man;
			contactPtr->GetWorldManifold(&man);
			b2Vec2& contact = man.points[0];
			if (static_cast<Body*>(cBody.GetUserData())->category.find("boid")) {
				los.friendAngles.emplace_back(cBody.GetAngle());
				los.friendPos.emplace_back(contact.x, contact.y);
			}
			else { los.badPos.emplace_back(contact.x, contact.y); }
		}
	}
}

Boid::Los::Los(float radius) : radius(radius) {}
