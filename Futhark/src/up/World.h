#pragma once
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <list>
#include "../out/OutUtility.h"
namespace fk {


	/* A class for managing the box 2d world.
	^ http://www.iforce2d.net/b2dtut/worlds
	[t3chma] */
	class World {
	  public:
		friend class WireRenderer;
		// A pointer to the actual b2 world object.
		b2World* b2Ptr{ nullptr };
		World();
		~World();
		/* Set the gravity of the world.
		[t3chma] */
		void setGravity(const float& xGravity, const float& yGravity);
		/* Updates the world.
		(timestep) Length of time passed to simulate in seconds.
		(velocityIterations) How strongly to correct velocity.
		(positionIterations) How strongly to correct position.
		[t3chma] */
		void update(const float& timestep, const int& velocityIterations, const int& positionIterations);
		// Information for drawing a point to the screen.
		struct Point {
			// Offset from position. 8
			Vec2 offset{ 0, 0 };
			// Hue of the point. 12
			Color color{ 255, 255, 255, 255 };
			// Axis of rotation for this point. 20
			Vec2 position{ 0, 0 };
			// Amount of rotation around the rotation axis. 24
			GLfloat rotationAngle{ 0.0 };
		};
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
		} m_contactListener;
		/* Setup the local vertex and index buffer using all the current bodies in the world.
		[t3chma] */
		void m_setupBuffers();
		/* Start the render pipeline.
		You probably should not even be calling this unless you are the renderer class.
		[t3chma] */
		void m_render();
		// Vertex buffer ID.
		GLuint m_vertexBufferObjectID{ 0 };
		// Array buffer ID.
		GLuint m_vertexArrayObjectID{ 0 };
		// Index buffer ID.
		GLuint m_indexBufferObjectID{ 0 };
		// The local vertex buffer to be sent to the GPU.
		std::vector<Point> m_vertexBuffer;
		// The local vertex buffer to be sent to the GPU.
		std::vector<GLuint> m_indexBuffer;
		// Circle definition
		Vec2 m_circleWireVectors[32];
	};

}