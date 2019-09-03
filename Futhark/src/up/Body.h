#pragma once
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <list>
namespace fk {


class World;

/* This is a class which is used to represent physical bodies which exist in a Box2D world.
=BASE= collision
See Box2D documentation for more information.
^ http://www.iforce2d.net/b2dtut/
[t3chma] */
class Body {
  public:
	friend World;
	// The Box2D object this class is built around.
	// ^ http://www.iforce2d.net/b2dtut/
	b2Body* b2Ptr{ nullptr };
	// Used for things like conditional collision events between certain categories of bodies.
	int species{ 0 };
	int team{ 0 };
	int squad{ 0 };
	int type{ 0 };
	int teir{ 0 };

	// Used to keep track of limbs.
	struct Limb {
		Limb(b2Fixture* b2Ptr, std::vector<glm::vec2> offsets, bool opens = false);
		// Used for things like conditional collision events between certain categories of bodies.
		std::string category{ "limb" };
		// Used to keep track of the b2Fixture pointer.
		b2Fixture* b2Ptr;
		// A list of the offsets for this limb.
		std::vector<glm::vec2> offsets;
		// Used to keep track of the type of line for rendering purposes.
		bool opens;
		// Convenient overload.
		bool operator == (const b2Shape::Type shape) const {
			return b2Ptr->GetType() == shape;
		}
	};
	// Used to keep track of limbs.
	std::list<Limb> limbs;
	// World of this body.
	World& world;
	Body() = delete;
	/* Constructor
	^ http://www.iforce2d.net/b2dtut/
	(world) b2World, see Box2D documentation.
	(type) Body type, see Box2D documentation.
	(xPosition) Horizontal position.
	(yPosition) Vertical position.
	(angle) Rotational offset.
	(fixedRotation) If the object is prevented from rotating.
	(bullet) If tunneling should be absolutely prevented, at the expense of more processing.
	[t3chma] */
	Body(
		World& world, b2BodyType type,
		float xPosition = 0.0f, float yPosition = 0.0f,
		float angle = 0.0f, bool fixedRotation = false, bool bullet = false,
		float angularDamping = 10, float linearDamping = 10
	);
	/* Moves this body into another one deleting the replaced body.
	(rhs) The body to move into.
	[t3chma] */
	void replace(Body& rhs);
	/* Destructor
	[t3chma] */
	virtual ~Body();
	/* Appends a circle to the body.
	(radius) The radius of the circle.
	(xOffset) Horizontal position relative to the body position.
	(yOffset) Vertical position relative to the body position.
	(fixtureDefPtr) Any additional (optional) info for the created limb.
	< The added limb.
	[t3chma] */
	virtual Limb& addCircleLimb(
		float radius,
		float xOffset = 0.0f, float yOffset = 0.0f,
		b2FixtureDef* fixtureDefPtr = nullptr
	) final;
	/* Appends a concave polygon to the body.
	IMPORTANT: Your polygon must be concave.
	(offsets) 8 or less of the polygon's corners specified in counter-clockwise order.
	(fixtureDefPtr) Any additional (optional) info for the created limb.
	< The added limb.
	[t3chma] */
	virtual Limb& addPolygonLimb(
		std::vector<glm::vec2>& offsets,
		b2FixtureDef* fixtureDefPtr = nullptr
	) final;
	/* Appends a rectangle to the body.
	(halfWidth) The distance from the center of the rectangle to the sides.
	(halfHeight) The distance from the center of the rectangle to the top or bottom.
	(xOffset) Horizontal position relative to the body position.
	(yOffset) Vertical position relative to the body position.
	(angle) Rotational offset relative to the body angle.
	(fixtureDefPtr) Any additional (optional) info for the created limb.
	< The added limb.
	[t3chma] */
	virtual Limb& addRectangleLimb(
		float halfWidth, float halfHeight,
		float xOffset = 0.0f, float yOffset = 0.0f,
		float angle = 0.0f,
		b2FixtureDef* fixtureDefPtr = nullptr
	) final;
	/* Appends a vertically oriented, composite capsule to the body.
	The capsule is made of 2 circles and a rectangle.
	(halfWidth) The distance from the center of the capsule to the sides.
	(halfHeight) The distance from the center of the capsule to the top or bottom.
	(xOffset) Horizontal position relative to the body position.
	(yOffset) Vertical position relative to the body position.
	(angle) Rotational offset relative to the body angle.
	(fixtureDefPtr) Any additional (optional) info for the created limb.
	< The added limb.
	[t3chma] */
	virtual void addCapsuleLimbs(
		float halfWidth, float halfHeight,
		float xOffset = 0.0f, float yOffset = 0.0f,
		float angle = 0.0f,
		b2FixtureDef* fixtureDefPtr = nullptr
	) final;
	/* Appends a equilateral to the body.
	(radius) The distance from the center of the equilateral to each of the corners.
	(sides) 8 or less sides for equilateral.
	(xOffset) Horizontal position relative to the body position.
	(yOffset) Vertical position relative to the body position.
	(angle) Rotational offset relative to the body angle.
	(fixtureDefPtr) Any additional (optional) info for the created limb.
	< The added limb.
	[t3chma] */
	virtual Limb& addEquilateralLimb(
		float radius, char sides,
		float xOffset, float yOffset,
		float angle,
		b2FixtureDef* fixtureDefPtr = nullptr
	) final;
	/* Appends a line to the body.
	IMPORTANT: No lines can collide with other lines.
	(offsets) The points which make up a line.
	(open) If the end of the line should connect back to the front.
	(fixtureDefPtr) Any additional (optional) info for the created limb.
	< The added limb.
	[t3chma] */
	virtual Limb& addLineLimb(
		std::vector<glm::vec2>& offsets,
		bool open,
		b2FixtureDef* fixtureDefPtr = nullptr
	) final;
  protected:
	/* Adds this to the wire batch.
	[t3chma] */
	//virtual void addToWireBatch(Futhark::Color* colorPtr, bool orientation = true) final;
	/* Intended for inheritance purposes where this code would execute when this body begins contact.
	[t3chma] */
	inline virtual void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) {};
	/* Intended for inheritance purposes where this code would execute when this body begins a collision.
	[t3chma] */
	inline virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	) {};
	/* Intended for inheritance purposes where this code would execute when this body ends a collision.
	[t3chma] */
	inline virtual void p_postCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2ContactImpulse* impulsePtr
	) {};
	/* Intended for inheritance purposes where this code would execute when this body ends contact.
	[t3chma] */
	inline virtual void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) {};
	// Convenient overload.
	inline bool operator == (const b2BodyType type) const {
		return b2Ptr->GetType() == type;
	}
};

}