#include "WarNeedle.h"


WarNeedle::WarNeedle(Map& map, PropDef& pd, glm::vec2& throwVector, Object* linkPtr) :
	Prop(map, pd), size(pd.size), throwVector(throwVector) {
	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.density = pd.density;
	fixtureDef.friction = pd.friction;
	fixtureDef.filter.categoryBits = 4;
	category = "prop";
	//addLineLimb(points, true);
	addRectangleLimb(
		pd.size / 2, 0.01,
		pd.size / 2, 0,
		0,
		&fixtureDef
	);
	b2Ptr->SetTransform(
		b2Ptr->GetPosition(),
		fk::makeAngle(glm::normalize(throwVector)) - fk::TAU / 2
	);
	b2Ptr->ApplyForceToCenter(b2Vec2(throwVector.x, throwVector.y), true);
	sprites.back().setColor(0,0,0,0);
	links.emplace_back(pd.position);
}

WarNeedle::~WarNeedle() {
	
}

void WarNeedle::p_postCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2ContactImpulse* impulsePtr
) {
	if (!welded) {
		weldPtr = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
		if (weldPtr->category == "rope") {
			weldPtr = nullptr;
		}
	}
}

void WarNeedle::update(fk::Camera* camPtr) {
	glm::vec2 lastLink = getPosition();
	for (auto&& link : links) {
		map.world.b2Ptr->RayCast(
			this,
			b2Vec2(lastLink.x, lastLink.y),
			b2Vec2(link.x, link.y)
		);
		lastLink = link;
	}
	if (stick && weldPtr) {
		//set the joint anchors at the arrow tip - should be good enough
		b2Vec2 worldCoordsAnchorPoint = weldPtr->b2Ptr->GetWorldPoint(b2Vec2(size / 2, 0));
		b2WeldJointDef weldJointDef;
		weldJointDef.bodyA = weldPtr->b2Ptr;
		weldJointDef.bodyB = b2Ptr;
		weldJointDef.localAnchorA = weldJointDef.bodyA->GetLocalPoint(worldCoordsAnchorPoint);
		weldJointDef.localAnchorB = weldJointDef.bodyB->GetLocalPoint(worldCoordsAnchorPoint);
		weldJointDef.referenceAngle = weldJointDef.bodyB->GetAngle() - weldJointDef.bodyA->GetAngle();
		map.world.b2Ptr->CreateJoint(&weldJointDef);
		weldPtr = nullptr;
		welded = true;
		auto knockback = glm::normalize(throwVector);
		b2Ptr->ApplyLinearImpulse(
			b2Vec2(knockback.x * 20, knockback.y * 20),
			b2Ptr->GetWorldCenter(),
			true
		);
	}
	if (stick) { --stick; }
}