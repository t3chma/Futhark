#include "World.h"
#include "Body.h"
namespace fk {


	World::World() {
		b2WorldPtr = new b2World(b2Vec2(0, 0));
		b2WorldPtr->SetContactListener(&m_contactListener);
	}
	World::~World() {
		delete b2WorldPtr;
	}
	void World::gravity(const float& xGravity, const float& yGravity) {
		b2WorldPtr->SetGravity(b2Vec2(xGravity, yGravity));
	}
	void World::update(const float& timestep, const int& velocityIterations, const int& positionIterations) {
		b2WorldPtr->Step(timestep, velocityIterations, positionIterations);
	}
	void World::M_ContactListener::BeginContact(b2Contact* contactPtr) {
		// Check if fixture A and B was a Futhark body
		if (
			contactPtr->GetFixtureA()->GetBody()->GetUserData()
			&& contactPtr->GetFixtureB()->GetBody()->GetUserData()
		) {
			Body* bodyPtr1 = static_cast<Body*>(contactPtr->GetFixtureA()->GetBody()->GetUserData());
			Body* bodyPtr2 = static_cast<Body*>(contactPtr->GetFixtureB()->GetBody()->GetUserData());
			bodyPtr1->p_beginCollision(contactPtr->GetFixtureB(), contactPtr->GetFixtureA(), contactPtr);
			bodyPtr2->p_beginCollision(contactPtr->GetFixtureA(), contactPtr->GetFixtureB(), contactPtr);
		};
	}
	void World::M_ContactListener::PreSolve(b2Contact* contactPtr, const b2Manifold* oldManifoldPtr) {
		// Check if fixture A and B was a Futhark body
		if (
			contactPtr->GetFixtureA()->GetBody()->GetUserData()
			&& contactPtr->GetFixtureB()->GetBody()->GetUserData()
		) {
			Body* bodyPtr1 = static_cast<Body*>(contactPtr->GetFixtureA()->GetBody()->GetUserData());
			Body* bodyPtr2 = static_cast<Body*>(contactPtr->GetFixtureB()->GetBody()->GetUserData());
			bodyPtr1->p_preCollisionAdjusting(
				contactPtr->GetFixtureB(),
				contactPtr->GetFixtureA(),
				contactPtr,
				oldManifoldPtr
			);
			bodyPtr2->p_preCollisionAdjusting(
				contactPtr->GetFixtureA(),
				contactPtr->GetFixtureB(),
				contactPtr,
				oldManifoldPtr
			);
		};
	}
	void World::M_ContactListener::PostSolve(b2Contact* contactPtr, const b2ContactImpulse* impulsePtr) {
		// Check if fixture A and B was a Futhark body
		if (
			contactPtr->GetFixtureA()->GetBody()->GetUserData()
			&& contactPtr->GetFixtureB()->GetBody()->GetUserData()
		) {
			Body* bodyPtr1 = static_cast<Body*>(contactPtr->GetFixtureA()->GetBody()->GetUserData());
			Body* bodyPtr2 = static_cast<Body*>(contactPtr->GetFixtureB()->GetBody()->GetUserData());
			bodyPtr1->p_postCollisionAdjusting(
				contactPtr->GetFixtureB(),
				contactPtr->GetFixtureA(),
				contactPtr,
				impulsePtr
			);
			bodyPtr2->p_postCollisionAdjusting(
				contactPtr->GetFixtureA(),
				contactPtr->GetFixtureB(),
				contactPtr,
				impulsePtr
			);
		};
	}
	void World::M_ContactListener::EndContact(b2Contact* contactPtr) {
		//check if fixture A and B was a Futhark body
		if (
			contactPtr->GetFixtureA()->GetBody()->GetUserData()
			&& contactPtr->GetFixtureB()->GetBody()->GetUserData()
		) {
			Body* bodyPtr1 = static_cast<Body*>(contactPtr->GetFixtureA()->GetBody()->GetUserData());
			Body* bodyPtr2 = static_cast<Body*>(contactPtr->GetFixtureB()->GetBody()->GetUserData());
			bodyPtr1->p_endCollision(contactPtr->GetFixtureB(), contactPtr->GetFixtureA(), contactPtr);
			bodyPtr2->p_endCollision(contactPtr->GetFixtureA(), contactPtr->GetFixtureB(), contactPtr);
		};
	}

}