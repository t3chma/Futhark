#include "Body.h"
#include "World.h"
#include "../base/Utility.h"
namespace fk {


	Body::Body(
		World& world, b2BodyType type,
		float xPosition, float yPosition, float angle,
		bool fixedRotaion, bool bullet,
		float angularDamping, float linearDamping
	) : world(world) {
		b2BodyDef bodyDef;
		bodyDef.type = type;
		bodyDef.fixedRotation = fixedRotaion;
		bodyDef.bullet = bullet;
		bodyDef.position.Set(xPosition, yPosition);
		bodyDef.angle = angle;
		bodyDef.angularDamping = angularDamping;
		bodyDef.linearDamping = linearDamping;

		b2Ptr = world.b2Ptr->CreateBody(&bodyDef);
		b2Ptr->SetUserData(this);
	}

	/* Moves this body into another one deleting the replaced body.
	[t3chma] */

	void Body::replace(Body & rhs) {
		rhs.world.b2Ptr->DestroyBody(rhs.b2Ptr);
		rhs.b2Ptr = b2Ptr;
		rhs.limbs = limbs;
		rhs.world = world;
		rhs.team = team;
		rhs.squad = squad;
		rhs.type = type;
		rhs.teir = teir;
		rhs.species = species;
		b2Ptr = nullptr;
		limbs.clear();
	}
	Body::~Body() {
		b2Ptr->GetWorld()->DestroyBody(b2Ptr);
	}
	Body::Limb& Body::addCircleLimb(
		float radius,
		float xOffset, float yOffset,
		b2FixtureDef* fixtureDefPtr
	) {
		bool destroyFixture = false;
		if (!fixtureDefPtr) { fixtureDefPtr = new b2FixtureDef(); destroyFixture = true; }
		std::vector<glm::vec2> offsets;
		offsets.emplace_back(xOffset, yOffset);

		b2CircleShape shape;
		shape.m_radius = radius;
		shape.m_p.Set(xOffset, yOffset);

		fixtureDefPtr->shape = &shape;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets);
		limbs.back().b2Ptr->SetUserData(&limbs.back());
		if (destroyFixture) { delete fixtureDefPtr; }
		return limbs.back();
	}
	Body::Limb& Body::addPolygonLimb(
		std::vector<glm::vec2>& offsets,
		b2FixtureDef* fixtureDefPtr
	) {
		if (offsets.size() > 8) { __debugbreak(); assert(false); }
		bool destroyFixture = false;
		if (!fixtureDefPtr) { fixtureDefPtr = new b2FixtureDef(); destroyFixture = true; }

		b2Vec2* bOffsets = new b2Vec2[offsets.size()];
		for (int i = 0; i < offsets.size(); ++i) {
			bOffsets[i].x = offsets[i].x;
			bOffsets[i].y = offsets[i].y;
		}

		b2PolygonShape shape;
		shape.Set(bOffsets, offsets.size());

		fixtureDefPtr->shape = &shape;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets);
		limbs.back().b2Ptr->SetUserData(&limbs.back());

		delete bOffsets;
		if (destroyFixture) { delete fixtureDefPtr; }
		return limbs.back();
	}
	Body::Limb& Body::addRectangleLimb(
		float halfWidth, float halfHeight,
		float xOffset, float yOffset,
		float angle,
		b2FixtureDef* fixtureDefPtr
	) {
		bool destroyFixture = false;
		if (!fixtureDefPtr) { fixtureDefPtr = new b2FixtureDef(); destroyFixture = true; }

		// Calculate corners
		std::vector<glm::vec2> offsets;
		offsets.resize(4);
		offsets[0].x = halfWidth + xOffset;
		offsets[0].y = halfHeight + yOffset;
		offsets[1].x = -halfWidth + xOffset;
		offsets[1].y = halfHeight + yOffset;
		offsets[2].x = -halfWidth + xOffset;
		offsets[2].y = -halfHeight + yOffset;
		offsets[3].x = halfWidth + xOffset;
		offsets[3].y = -halfHeight + yOffset;

		// Rotate
		offsets[0] = rotatePoint(offsets[0], angle);
		offsets[1] = rotatePoint(offsets[1], angle);
		offsets[2] = rotatePoint(offsets[2], angle);
		offsets[3] = rotatePoint(offsets[3], angle);

		b2PolygonShape shape;
		shape.SetAsBox(halfWidth, halfHeight, b2Vec2(xOffset, yOffset), angle);

		fixtureDefPtr->shape = &shape;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets);
		limbs.back().b2Ptr->SetUserData(&limbs.back());
		if (destroyFixture) { delete fixtureDefPtr; }
		return limbs.back();
	}
	void Body::addCapsuleLimbs(
		float halfWidth, float halfHeight,
		float xOffset, float yOffset,
		float angle,
		b2FixtureDef* fixtureDefPtr
	) {
		bool destroyFixture = false;
		if (!fixtureDefPtr) { fixtureDefPtr = new b2FixtureDef(); destroyFixture = true; }

		// Rectangle

		addRectangleLimb(halfWidth, halfHeight, xOffset, yOffset, angle, fixtureDefPtr);

		// Cirlce 1

		glm::vec2 center1(xOffset, yOffset + halfHeight);

		center1 = rotatePoint(center1, angle);
		std::vector<glm::vec2> offsets1;
		offsets1.push_back(center1);

		b2CircleShape shape1;
		shape1.m_radius = halfWidth;
		shape1.m_p.Set(center1.x, center1.y);

		fixtureDefPtr->shape = &shape1;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets1);

		// Circle 2

		glm::vec2 center2(xOffset, yOffset - halfHeight);

		center2 = rotatePoint(center2, angle);
		std::vector<glm::vec2> offsets2;
		offsets2.push_back(center2);

		b2CircleShape shape2;
		shape2.m_radius = halfWidth;
		shape2.m_p.Set(center2.x, center2.y);

		fixtureDefPtr->shape = &shape2;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets1);
		if (destroyFixture) { delete fixtureDefPtr; }
	}
	Body::Limb& Body::addEquilateralLimb(
		float radius, char sides,
		float xOffset, float yOffset,
		float angle,
		b2FixtureDef* fixtureDefPtr
	) {
		if (sides > 8) { __debugbreak(); assert(false); }
		bool destroyFixture = false;
		if (!fixtureDefPtr) { fixtureDefPtr = new b2FixtureDef(); destroyFixture = true; }

		std::vector<glm::vec2> offsets;
		offsets.resize(sides);
		for (int i = 0; i < sides; ++i) {
			float pointAngle = ((float)i / sides) * TAU + angle;
			offsets[i].x = cos(pointAngle) + xOffset;
			offsets[i].y = sin(pointAngle) + yOffset;
		}

		b2Vec2* offsetsArray = new b2Vec2[offsets.size()];
		for (int i = 0; i < offsets.size(); ++i) {
			offsetsArray[i].x = offsets[i].x * radius;
			offsetsArray[i].y = offsets[i].y * radius;
		}

		b2PolygonShape shape;
		shape.Set(offsetsArray, offsets.size());

		fixtureDefPtr->shape = &shape;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets);

		delete offsetsArray;
		if (destroyFixture) { delete fixtureDefPtr; }
		return limbs.back();
	}
	Body::Limb& Body::addLineLimb(
		std::vector<glm::vec2>& offsets,
		bool open,
		b2FixtureDef* fixtureDefPtr
	) {
		bool destroyFixture = false;
		if (!fixtureDefPtr) { fixtureDefPtr = new b2FixtureDef(); destroyFixture = true; }

		b2Vec2* bOffsets = new b2Vec2[offsets.size()];
		for (int i = 0; i < offsets.size(); ++i) {
			bOffsets[i].x = offsets[i].x;
			bOffsets[i].y = offsets[i].y;
		}

		b2ChainShape shape;
		if (open) { shape.CreateChain(bOffsets, offsets.size()); }
		else { shape.CreateLoop(bOffsets, offsets.size()); }

		fixtureDefPtr->shape = &shape;

		limbs.emplace_back(b2Ptr->CreateFixture(fixtureDefPtr), offsets, open);
		if (destroyFixture) { delete fixtureDefPtr; }
		delete bOffsets;
		return limbs.back();
	}

	Body::Limb::Limb(b2Fixture* b2Ptr, std::vector<glm::vec2> offsets, bool opens) :
		b2Ptr(b2Ptr),
		offsets(offsets),
		opens(opens)
	{
		b2Ptr->SetUserData(this);
	}

}