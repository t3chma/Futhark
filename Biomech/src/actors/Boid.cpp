#include "Boid.h"
#include "../spectrals/Equipment.h"


void Boid::primeGun(int option) { if (p_gunPtr) {
	Target t;
	t.location = glm::normalize(p_looking - glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y));
	p_gunPtr->prime(option, this, t);
} }

void Boid::releaseGun() { if (p_gunPtr) { p_gunPtr->release(); } }

void Boid::primeGadget(int option) { if (p_gadgetPtr) {
	Target t;
	t.location = glm::normalize(p_looking - glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y));
	p_gadgetPtr->prime(option, this, t);
} }

inline void Boid::releaseGadget() { if (p_gadgetPtr) { p_gadgetPtr->release(); } }

Boid::Boid(fk::SpriteBatch& sb, fk::World& world, Boid::Def bd)
	: Body(world, bd.bd), Image(sb), los(bd.sightRadius), pw(bd.pw)
{
	type = (int)Type::SWARM;
	addCircleLimb(bd.sightRadius);
	limbs.front().category = "s";
	goals.resize(2);
}
Boid::~Boid() {

}
void Boid::attack(Body& body) {
	auto d = (&body == statEffects.markPtr) ? damage * 2 : damage;
	body.damage(d);
	if (&body == statEffects.bondPtr) { p_health -= d / 2; }
}
void Boid::p_think(fk::UserInput& ui) {

}
void Boid::think(fk::UserInput & ui) {
	p_think(ui);
	if (p_health > 0 && type != (int)Type::PLAYER) {
		// Get goals
		for (auto&& g : goals) {
			if (!g.resolved) {
				goal = g.location;
				if (g.bodyPtr && g.bodyPtr->connect(this)) {
					g.resolved = g.bodyPtr->interact(this);
				} else {
					// Get my position and angle
					b2Vec2 bPos = b2Ptr->GetPosition();
					glm::vec2 myPos(bPos.x, bPos.y);
					float myAng = b2Ptr->GetAngle();
					if (std::isnan(bPos.x) || std::isnan(bPos.y) || std::isnan(myAng)) { return; }

					// Reset goal vectors.
					gv.alignment.x = 0;
					gv.alignment.y = 0;
					gv.seperation.x = 0;
					gv.seperation.y = 0;
					gv.cohesion.x = 0;
					gv.cohesion.y = 0;
					gv.aversion.x = 0;
					gv.aversion.y = 0;
					gv.seek.x = 0;
					gv.seek.y = 0;
					gv.target.x = 0;
					gv.target.y = 0;
					// Get alignment.
					for (auto&& f : los.friends) {
						gv.alignment += glm::length(f.direction) > 1 ? glm::normalize(f.direction) : f.direction;
					}
					gv.alignment *= los.friends.size() ? 1.0 / los.friends.size() : 0.0;
					// Get seperation, cohesion and seek.
					for (auto&& f : los.friends) {
						glm::vec2 s = f.position - myPos;
						gv.seperation -= (s.x || s.y) ? glm::normalize(s) / glm::length(s) : s;
						gv.cohesion += f.position;
					}
					gv.seek.x = (gv.cohesion.x + myPos.x) / (los.friends.size() + 1);
					gv.seek.y = (gv.cohesion.y + myPos.y) / (los.friends.size() + 1);
					gv.seek = goal - gv.seek;
					gv.seek = (gv.seek.x || gv.seek.y) ? glm::normalize(gv.seek) : gv.seek;
					gv.cohesion *= los.friends.size() ? 1.0 / los.friends.size() : 0.0;
					gv.cohesion -= (gv.cohesion.x || gv.cohesion.y) ? myPos : glm::vec2(0);
					// Get aversion
					for (auto&& p : los.badPositions) {
						glm::vec2 a = p - myPos;
						gv.aversion -= (a.x || a.y) ? glm::normalize(a) / (
							glm::length(a) * glm::length(a)* glm::length(a)
							) : a;
					}
					gv.aversion *= los.badPositions.size() ? 1.0 / los.badPositions.size() : 0.0;
					//gv.aversion = fk::rotatePoint(gv.aversion, fk::TAU/4.0);
					// Get target
					gv.target = goal - myPos;
					gv.target = (gv.target.x || gv.target.y) ? glm::normalize(gv.target) : gv.target;

					// Get new direction
					static fk::Random rangen;
					gv.alignment *= pw.alignment * !(bool)statEffects.blind;
					gv.seperation *= pw.seperation * !(bool)statEffects.blind;
					gv.cohesion *= pw.cohesion * !(bool)statEffects.blind;
					gv.seek *= pw.seek * !(bool)statEffects.blind;
					gv.target *= pw.target * !(bool)statEffects.blind;
					gv.snap = gv.target * (pw.snap / glm::length(goal - myPos) * !(bool)statEffects.blind);
					gv.aversion *= pw.aversion * !(bool)statEffects.blind;
					p_direction =
						glm::vec2(rangen.getFloat(-1, 1), rangen.getFloat(-1, 1)) * pw.studder
						+ gv.alignment + gv.seperation + gv.cohesion
						+ gv.seek + gv.target + gv.snap
						+ gv.aversion;
					p_looking = p_direction;
					// Move and rotate toward direction
					if (p_direction.x || p_direction.y) {
						auto direction = glm::normalize(p_direction);
						double dAngle = fk::makeAngle(direction);
						b2Ptr->SetTransform(b2Ptr->GetPosition(), dAngle);
						auto speed = glm::length(p_direction);
						if (speed > p_speed) { direction *= p_speed; }
						else { direction = p_direction; }
						b2Ptr->ApplyForceToCenter(b2Vec2(direction.x, direction.y), true);
					}
					// Clear Contacts
					los.badPositions.clear();
					los.friends.clear();
				}
				break;
			}
		}
	}
}
void Boid::draw() {

}

void Boid::setGun(Equipment* ePtr) {
	delete p_gunPtr;
	p_gunPtr = ePtr;
	p_gunPtr->userPtr = this;
}

Equipment* Boid::getGun() {
	return p_gunPtr;
}

void Boid::setGadget(Equipment* ePtr) {
	delete p_gadgetPtr;
	p_gadgetPtr = ePtr;
	p_gadgetPtr->userPtr = this;
}

Equipment* Boid::getGatget() {
	return p_gadgetPtr;
}

void Boid::p_preCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2Manifold* oldManifoldPtr
) {
	// sight
	if (static_cast<Limb*>(myFixturePtr->GetUserData())->category == "s") {
		contactPtr->SetEnabled(false);
		if (
			!collisionFixturePtr->IsSensor() &&
			static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s"
		) {
			b2WorldManifold worldManifold;
			contactPtr->GetWorldManifold(&worldManifold);
			Body& b = *static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
			if (b.type == (int)Type::PROP || b.team != team || b.squad != squad) {
				los.badPositions.emplace(worldManifold.points[0].x, worldManifold.points[0].y);
			} else {
				los.friends.emplace(
					static_cast<Boid*>(collisionFixturePtr->GetBody()->GetUserData())->getDirection().x,
					static_cast<Boid*>(collisionFixturePtr->GetBody()->GetUserData())->getDirection().y,
					worldManifold.points[0].x,
					worldManifold.points[0].y
				);
			}
		}
	}
	// body
	else {
		Body& b = *static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
		if (b.type != (int)Type::PROP && b.team == team) {
			contactPtr->SetEnabled(false);
		}
	}
};

void Boid::p_postCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2ContactImpulse* impulsePtr
) {
};

Boid::LineOfSight::LineOfSight(float radius) : radius(radius) {}