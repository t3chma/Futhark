#include "Body.h"
#include "World.h"
#include "../base/Utility.h"
namespace fk {


	Body::Body(
		World& world, b2BodyType type,
		float xPosition, float yPosition, float angle,
		bool fixedRotaion, bool bullet
	) {
		b2BodyDef bodyDef;
		bodyDef.type = type;
		bodyDef.fixedRotation = fixedRotaion;
		bodyDef.bullet = bullet;
		bodyDef.position.Set(xPosition, yPosition);
		bodyDef.angle = angle;

		b2BodyPtr = world.b2WorldPtr->CreateBody(&bodyDef);
		b2BodyPtr->SetUserData(this);
	}
	Body::~Body() {
		b2BodyPtr->GetWorld()->DestroyBody(b2BodyPtr);
	}
	void Body::addCircleLimb(float radius, float xOffset, float yOffset) {
		std::vector<glm::vec2> offsets;
		offsets.emplace_back(xOffset, yOffset);
		this->offsets.push_back(offsets);

		b2CircleShape shape;
		shape.m_radius = radius;
		shape.m_p = b2Vec2(xOffset, yOffset);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.userData = nullptr;
		++numberOfLimbs;

		shapes.push_back('c');
	}
	void Body::addPolygonLimb(std::vector<glm::vec2>& offsets) {
		if (offsets.size() > 8) { return; }

		this->offsets.push_back(offsets);

		b2Vec2* bOffsets = new b2Vec2[offsets.size()];
		for (int i = 0; i < offsets.size(); ++i) {
			bOffsets[i].x = offsets[i].x;
			bOffsets[i].y = offsets[i].y;
		}

		b2PolygonShape shape;
		shape.Set(bOffsets, offsets.size());

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.userData = nullptr;
		++numberOfLimbs;

		shapes.push_back('p');

		delete bOffsets;
	}
	void Body::addRectangleLimb(
		float halfWidth, float halfHeight,
		float xOffset, float yOffset,
		float angle
	) {
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
		this->offsets.push_back(offsets);

		b2PolygonShape shape;
		shape.SetAsBox(halfWidth, halfHeight, b2Vec2(xOffset, yOffset), angle);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.userData = nullptr;
		++numberOfLimbs;

		shapes.push_back('p');
	}
	void Body::addCapsuleLimbs(
		float halfWidth, float halfHeight,
		float xOffset, float yOffset, float angle
	) {
		// Rectangle

		addRectangleLimb(halfWidth, halfHeight, xOffset, yOffset, angle);

		// Cirlce 1

		glm::vec2 center1(xOffset, yOffset + halfHeight);

		center1 = rotatePoint(center1, angle);
		std::vector<glm::vec2> offsets1;
		offsets1.push_back(center1);
		this->offsets.push_back(offsets1);

		b2CircleShape shape1;
		shape1.m_radius = halfWidth;
		shape1.m_p = b2Vec2(center1.x, center1.y);

		b2FixtureDef fixtureDef1;
		fixtureDef1.shape = &shape1;
		fixtureDef1.density = 1.0f;
		fixtureDef1.friction = 0.3f;
		fixtureDef1.userData = nullptr;
		++numberOfLimbs;

		shapes.push_back('c');

		// Circle 2

		glm::vec2 center2(xOffset, yOffset - halfHeight);

		center2 = rotatePoint(center2, angle);
		std::vector<glm::vec2> offsets2;
		offsets2.push_back(center2);
		this->offsets.push_back(offsets2);

		b2CircleShape shape2;
		shape2.m_radius = halfWidth;
		shape2.m_p = b2Vec2(center2.x, center2.y);

		b2FixtureDef fixtureDef2;
		fixtureDef2.shape = &shape2;
		fixtureDef2.density = 1.0f;
		fixtureDef2.friction = 0.3f;
		++numberOfLimbs;

		shapes.push_back('c');
	}
	void Body::addEquilateralLimb(
		float radius, char sides,
		float xOffset, float yOffset,
		float angle
	) {
		if (sides > 8) { return; }

		std::vector<glm::vec2> offsets;
		offsets.resize(sides);
		for (int i = 0; i < sides; ++i) {
			float pointAngle = ((float)i / sides) * TAU + angle;
			offsets[i].x = cos(pointAngle) + xOffset;
			offsets[i].y = sin(pointAngle) + yOffset;
		}
		this->offsets.push_back(offsets);

		b2Vec2* offsetsArray = new b2Vec2[offsets.size()];
		for (int i = 0; i < offsets.size(); ++i) {
			offsetsArray[i].x = offsets[i].x * radius;
			offsetsArray[i].y = offsets[i].y * radius;
		}

		b2PolygonShape shape;
		shape.Set(offsetsArray, offsets.size());

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.userData = nullptr;
		++numberOfLimbs;

		shapes.push_back('p');

		delete offsetsArray;
	}
	void Body::addLineLimb(std::vector<glm::vec2>& offsets, bool open) {
		this->offsets.push_back(offsets);

		b2Vec2* bOffsets = new b2Vec2[offsets.size()];
		for (int i = 0; i < offsets.size(); ++i) {
			bOffsets[i].x = offsets[i].x;
			bOffsets[i].y = offsets[i].y;
		}

		b2ChainShape shape;
		if (open) {
			shape.CreateChain(bOffsets, offsets.size());
			opens.push_back(true);
		} else {
			shape.CreateLoop(bOffsets, offsets.size());
		}

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.userData = nullptr;
		++numberOfLimbs;

		shapes.push_back('l');

		delete bOffsets;
	}
	//void Body::addToWireBatch(Futhark::Color* colorPtr, bool orientation) {
	//	 Futhark::Color color;
	//	 switch (p_bodyPtr->GetType()) {
	//	 case b2_dynamicBody:
	//	 	color = Futhark::PresetColors::ORANGE;
	//	 	break;
	//	 case b2_kinematicBody:
	//	 	color = Futhark::PresetColors::COBALT;
	//	 	break;
	//	 case b2_staticBody:
	//	 	color = Futhark::PresetColors::CREAM_GREEN;
	//	 	break;
	//	 }
	//	 if (colorPtr != nullptr) {
	//	 	color = *colorPtr;
	//	 }
	//	 for (int i = 0; i < p_shapes.size(); ++i) {
	//	 	switch (p_shapes[i]) {
	//	 	case 'c':
	//	 		Futhark::WireBatch::addCircle(
	//	 			glm::vec2(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y),
	//	 			p_limbPtrs[i]->GetShape()->m_radius,
	//	 			p_offsets[i][0],
	//	 			p_bodyPtr->GetAngle(),
	//	 			color,
	//	 			orientation
	//	 		);
	//	 		break;
	//	 	case 'p':
	//	 		Futhark::WireBatch::addPolygon(
	//	 			glm::vec2(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y),
	//	 			p_offsets[i],
	//	 			p_bodyPtr->GetAngle(),
	//	 			color,
	//	 			orientation
	//	 		);
	//	 		break;
	//	 	case 'l':
	//	 		if (p_opens[i]) {
	//	 			Futhark::WireBatch::addLine(
	//	 				glm::vec2(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y),
	//	 				p_offsets[i],
	//	 				p_bodyPtr->GetAngle(),
	//	 				color,
	//	 				orientation
	//	 			);
	//	 		}
	//	 		else {
	//	 			Futhark::WireBatch::addPolygon(
	//	 				glm::vec2(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y),
	//	 				p_offsets[i],
	//	 				p_bodyPtr->GetAngle(),
	//	 				color,
	//	 				orientation
	//	 			);
	//	 		}
	//	 		break;
	//	 	}
	//	 }
	//}

}