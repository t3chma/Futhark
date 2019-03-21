#include "Grunt.h"
#include "Player.h"


Grunt::Grunt(Map& map, GruntDef& gd) : Actor(map, gd.ad, *(new Idle(*this)), new Grunt::Agro(*this)) {
	// Misc
	movement.speed = 1.8;
	health = 5;
	type = "grunt";
	// Graphics
	sprites.front().setColor(255, 255, 0, 255);
	sprites.emplace_back(map.dynamicObjectSprites, gd.swipe);
	sprites.back().setDimensions(0.8, 1.6);
	sprites.back().setColor(255, 255, 0, 255);
	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	addRectangleLimb(gd.ad.size / 4, gd.ad.size, 0, gd.ad.size, 0, &fixtureDef).category = "Attack";
}
Grunt::~Grunt() {
	
}
void Grunt::updateSprite() {
	int alpha;
	alpha = sprites.back().getCanvasRef().color.a - 10;
	if (alpha < 0) { alpha = 0; }
	sprites.back().setColor(255, 255, 255, alpha);
	Actor::updateSprite();
}
void Grunt::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else if (static_cast<Limb*>(myFixturePtr->GetUserData())->category == "Attack") {
		m_swipePtrs.insert(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
	}
}
void Grunt::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else if (static_cast<Limb*>(myFixturePtr->GetUserData())->category == "Attack") {
		m_swipePtrs.erase(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
	}
}


void Grunt::Agro::enter() {
	actorPtr->sprites.front().setColor(255, 255, 255, 255);
}
void Grunt::Agro::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	bool canSee = actorPtr->inLOS(actorPtrs[0], 4, "actor");
	if (!actorPtrs[0]->movement.dodging && canSee) {
		actorPtr->targetInfo.lastKnownPos = actorPtrs[0]->getPosition();
	}
	glm::vec2 targetVec = actorPtr->getPosition() - actorPtr->targetInfo.lastKnownPos;
	if (targetVec.x || targetVec.y) { actorPtr->los.faceDirection = glm::normalize(targetVec); }

	if (!actorPtr->getPathFindingData().staleData) {
		actorPtr->movement.direction = actorPtr->movement.speed * actorPtr->getNextPathPointDirection(
			actorPtr->targetInfo.lastKnownPos
		);
		if (actorPtr->getPathFindingData().done) {
			float length = glm::length(targetVec);
			if (!canSee || length > range) {
				actorPtr->startAStar(actorPtr->targetInfo.lastKnownPos);
			} else {
				glm::vec2 targ(0);
				int division = 5;
				bool redo(false);
				do {
					float cost(0);
					// Rotate
					targ = fk::rotatePoint(
						actorPtr->targetInfo.lastKnownPos - actorPtr->getPosition(),
						fk::TAU / division * direction
					) + actorPtr->getPosition();
					if (length < range / 2) {
						targ += glm::vec2(
							actorPtr->los.faceDirection.x * 3, actorPtr->los.faceDirection.x * 3
						);
					}
					// Evaluate cost
					const Terrain* tilePtr = actorPtr->map.getTilePtr(targ);
					if (tilePtr) {
						cost = actorPtr->getFloorCost(tilePtr->floor)
							+ actorPtr->getFluidCost(tilePtr->fluid)
							+ actorPtr->getVaporCost(tilePtr->vapor)
							+ tilePtr->health * 2;
					} else {
						cost = 10000000;
					}
					redo = cost > 10 || !actorPtr->inLOS(targ);
					if (redo) {
						// Swap direction
						if (direction < 0) { division += 1; }
						direction *= -1;
						if (division == 10) { break; }
					}
				} while (redo);
				actorPtr->startAStar(targ);
			}
		} else {
			actorPtr->advanceAStar();
		}
	} else {
		if (actorPtr->getPathFindingData().done) {
			actorPtr->startAStar(actorPtr->targetInfo.lastKnownPos);
		}
		else { actorPtr->advanceAStar(); }
		actorPtr->movement.direction = glm::vec2(0);
	}
	justEntered = false;

	//if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	if (glm::length(targetVec) < 0.5) {
		actorPtr->setState(new Search(*actorPtr));
	} else if (!rangen.getInt(0, 500)) {
		actorPtr->setState(new Charge(*actorPtr));
	}
}


void Grunt::Charge::enter() {
	actorPtr->sprites.front().setColor(255, 200, 200, 255);
}
void Grunt::Charge::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	bool canSee = actorPtr->inLOS(actorPtrs[0], 4, "actor");
	if (!actorPtrs[0]->movement.dodging) {
		actorPtr->targetInfo.lastKnownPos = actorPtrs[0]->getPosition();
	}
	glm::vec2 targetVec = actorPtr->getPosition() - actorPtr->targetInfo.lastKnownPos;
	if (targetVec.x || targetVec.y) { actorPtr->los.faceDirection = glm::normalize(targetVec); }

	if (!actorPtr->getPathFindingData().staleData) {
		actorPtr->movement.direction = actorPtr->movement.speed * actorPtr->getNextPathPointDirection(
			actorPtr->targetInfo.lastKnownPos
		);
		if (actorPtr->getPathFindingData().done) {
			actorPtr->startAStar(actorPtr->targetInfo.lastKnownPos);
		}
		else { actorPtr->advanceAStar(); }
	} else {
		if (actorPtr->getPathFindingData().done) {
			actorPtr->startAStar(actorPtr->targetInfo.lastKnownPos);
		}
		else { actorPtr->advanceAStar(); }
		actorPtr->movement.direction = glm::vec2(0);
	}
	justEntered = false;

	//if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	if (!canSee) { actorPtr->setState(new Agro(*actorPtr)); }
	else if (glm::length(targetVec) < 0.8) {
		actorPtr->setState(new Attack(*actorPtr));
	}
}


void Grunt::Attack::enter() {
	actorPtr->sprites.front().setColor(255, 100, 100, 255);
	actorPtr->sprites.back().setColor(255, 0, 0, 255);
	actorPtr->sprites.back().setPosition(
		actorPtr->b2Ptr->GetPosition().x, actorPtr->b2Ptr->GetPosition().y
	);
	actorPtr->sprites.back().setRotationAxis(
		actorPtr->b2Ptr->GetPosition().x, actorPtr->b2Ptr->GetPosition().y
	);
	actorPtr->sprites.back().getCanvasRef().rotationAngle = fk::makeAngle(
		actorPtr->los.faceDirection
	);
}
void Grunt::Attack::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	actorPtr->look(actorPtr->los.faceDirection);
	for (auto&& hitBodyPtr : static_cast<Grunt*>(actorPtr)->m_swipePtrs) {
		hitBodyPtr->b2Ptr->ApplyLinearImpulse(
			b2Vec2(-actorPtr->los.faceDirection.x * 10, -actorPtr->los.faceDirection.y * 10),
			hitBodyPtr->b2Ptr->GetPosition(),
			true
		);
		if (
			static_cast<Object*>(hitBodyPtr)->category != "actor"
			|| !static_cast<Actor*>(hitBodyPtr)->movement.dodging
		) {
			static_cast<Object*>(hitBodyPtr)->health -= 1;
			static_cast<Object*>(hitBodyPtr)->hit = true;
		}
	}
	actorPtr->setState(new Agro(*actorPtr));
}
