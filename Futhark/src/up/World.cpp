#include "World.h"
#include "Body.h"
#include "../out/Error.h"
namespace fk {


	World::World() {
		b2WorldPtr = new b2World(b2Vec2(0, 0));
		b2WorldPtr->SetContactListener(&m_contactListener);
		TRY_GL(glGenVertexArrays(1, &m_vertexArrayObjectID));
		TRY_GL(glBindVertexArray(m_vertexArrayObjectID));
		TRY_GL(glGenBuffers(1, &m_vertexBufferObjectID));
		TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectID));
		TRY_GL(glGenBuffers(1, &m_indexBufferObjectID));
		TRY_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObjectID));
		TRY_GL(glEnableVertexAttribArray(0));
		TRY_GL(glEnableVertexAttribArray(1));
		TRY_GL(glEnableVertexAttribArray(2));
		TRY_GL(glEnableVertexAttribArray(3));
		TRY_GL(glVertexAttribPointer(
			0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, offset)
		));
		TRY_GL(glVertexAttribPointer(
			1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Point), (void*)offsetof(Point, color)
		));
		TRY_GL(glVertexAttribPointer(
			2, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position)
		));
		TRY_GL(glVertexAttribPointer(
			3, 1, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, rotationAngle)
		));
		TRY_GL(glBindVertexArray(0));
		TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		for (int i = 0; i < 32; ++i) {
			float angle = ((float)i / 32) * fk::TAU;
			m_circleWireVectors[i].x = cos(angle);
			m_circleWireVectors[i].y = sin(angle);
		}
	}
	World::~World() {
		delete b2WorldPtr;
	}
	void World::setGravity(const float& xGravity, const float& yGravity) {
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
	void World::m_setupBuffers() {
		for (b2Body* bodyPtr = b2WorldPtr->GetBodyList(); bodyPtr; bodyPtr = bodyPtr->GetNext()) {
			fk::Body& body = *static_cast<fk::Body*>(bodyPtr->GetUserData());
			fk::Color color;
			switch (bodyPtr->GetType()) {
			  case b2_dynamicBody: color = fk::PresetColors::ORANGE;
			  break;
			  case b2_kinematicBody: color = fk::PresetColors::COBALT;
			  break;
			  case b2_staticBody: color = fk::PresetColors::CREAM_GREEN;
			  break;
			}
			if (body.opens.empty()) { continue; }
			b2Fixture* fixturePtr = body.b2BodyPtr->GetFixtureList();
			for (int i = 0; i < body.shapes.size(); ++i, fixturePtr = fixturePtr->GetNext()) {
				if (!fixturePtr) { break; }
				unsigned int numberOfOffsets{ 0 };
				unsigned int verticesSize{ m_vertexBuffer.size() };
				// If circle
				if (body.shapes[i] == 'c') {
					// Add space for new verticies
					numberOfOffsets = 32;
					m_vertexBuffer.resize(verticesSize + numberOfOffsets + 1);
					// Polulate
					for (int j = 0; j < numberOfOffsets; ++j) {
						m_vertexBuffer[verticesSize + j].offset.x =
							m_circleWireVectors[j].x * fixturePtr->GetShape()->m_radius;
						m_vertexBuffer[verticesSize + j].offset.y =
							m_circleWireVectors[j].y * fixturePtr->GetShape()->m_radius;
						m_vertexBuffer[verticesSize + j].position.x =
							body.b2BodyPtr->GetPosition().x;
						m_vertexBuffer[verticesSize + j].position.y =
							body.b2BodyPtr->GetPosition().y;
						m_vertexBuffer[verticesSize + j].rotationAngle = body.b2BodyPtr->GetAngle();
						m_vertexBuffer[verticesSize + j].color = color;
					}
				// If not circle
				} else {
					// Add space for new verticies
					numberOfOffsets = body.offsets.size();
					m_vertexBuffer.resize(verticesSize + numberOfOffsets + 1);
					// Populate
					for (int j = 0; j < numberOfOffsets; ++j) {
						m_vertexBuffer[verticesSize + j].offset.x = body.offsets[i][j].x;
						m_vertexBuffer[verticesSize + j].offset.y = body.offsets[i][j].y;
						m_vertexBuffer[verticesSize + j].position.x =
							body.b2BodyPtr->GetPosition().x;
						m_vertexBuffer[verticesSize + j].position.y =
							body.b2BodyPtr->GetPosition().y;
						m_vertexBuffer[verticesSize + j].rotationAngle = body.b2BodyPtr->GetAngle();
						m_vertexBuffer[verticesSize + j].color = color;
					}
				}
				// Set up indices for indexed drawing
				unsigned int indicesSize{ m_indexBuffer.size() };
				m_indexBuffer.reserve(m_indexBuffer.size() + (numberOfOffsets + 1) * 2);
				for (int i = 0; i < numberOfOffsets - 1; ++i) {
					m_indexBuffer.push_back(verticesSize + i);
					m_indexBuffer.push_back(verticesSize + i + 1);
				}
				// Close the shape if it isn't open
				if (!body.opens[i]) {
					m_indexBuffer.push_back(verticesSize + numberOfOffsets - 1);
					m_indexBuffer.push_back(verticesSize);
				}
				// Show orienttion
				m_vertexBuffer[verticesSize + numberOfOffsets].offset.x =
					body.b2BodyPtr->GetPosition().x;
				m_vertexBuffer[verticesSize + numberOfOffsets].offset.y =
					body.b2BodyPtr->GetPosition().y;
				m_vertexBuffer[verticesSize + numberOfOffsets].position.x = 0;
				m_vertexBuffer[verticesSize + numberOfOffsets].position.y = 0;
				m_vertexBuffer[verticesSize + numberOfOffsets].rotationAngle = 0;
				m_vertexBuffer[verticesSize + numberOfOffsets].color.a =
					body.shapes[i] == 'l' ? 0 : 255; // If this is a line fade out the orientation line
				m_indexBuffer.push_back(verticesSize + numberOfOffsets);
				m_indexBuffer.push_back(verticesSize);
			}
		}
	}
	void World::m_render() {
		m_setupBuffers();
		// Buffer data in GPU.
		TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObjectID));
		// Describe data and throw out old data (orphaning) for speed
		// https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml
		TRY_GL(
			glBufferData(
				GL_ARRAY_BUFFER,
				sizeof(Point) * m_vertexBuffer.size(),
				m_vertexBuffer.data(),
				GL_DYNAMIC_DRAW
			)
		);
		TRY_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		TRY_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObjectID));
		TRY_GL(
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,
				sizeof(GLuint) * m_indexBuffer.size(),
				m_indexBuffer.data(),
				GL_DYNAMIC_DRAW
			)
		);
		TRY_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		TRY_GL(glBindVertexArray(m_vertexArrayObjectID));
		TRY_GL(glDrawElements(GL_LINES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0));
		TRY_GL(glBindVertexArray(0));
		m_vertexBuffer.clear();
		m_indexBuffer.clear();
	}

}