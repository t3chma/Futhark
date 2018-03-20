#pragma once
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <list>
namespace fk {


	/* A class for managing the box 2d world.
	^ http://www.iforce2d.net/b2dtut/worlds
	[t3chma] */
	class World {
	public:
		// A pointer to the actual b2 world object.
		b2World* b2WorldPtr{ nullptr };
		World();
		~World();
		/* Set the gravity of the world.
		[t3chma] */
		void gravity(const float& xGravity, const float& yGravity);
		/* Updates the world.
		(timestep) Length of time passed to simulate in seconds.
		(velocityIterations) How strongly to correct velocity.
		(positionIterations) How strongly to correct position.
		[t3chma] */
		void update(const float& timestep, const int& velocityIterations, const int& positionIterations);
	private:
		/* A custom contact listener for handling collisions for this class.
		^ http://www.iforce2d.net/b2dtut/collision-callbacks
		[t3chma] */
		class M_ContactListener : public b2ContactListener {
			/* Called when a contact starts.
			[t3chma] */
			virtual void BeginContact(b2Contact* contact) override;
			/* Called before collision processing,
			[t3chma] */
			virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
			/* Called after collision processing.
			[t3chma] */
			virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
			/* Called when contact ends
			[t3chma] */
			virtual void EndContact(b2Contact* contact) override;
		};
		// Used for handling collisions between Futhark Bodies.
		// ^ Futhark::Body
		M_ContactListener m_contactListener;
	};

}