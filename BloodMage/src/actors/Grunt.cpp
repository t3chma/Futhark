#include "Grunt.h"
#include "Player.h"


Grunt::Grunt(Map& map, GruntDef& gd) : Actor(map, gd.ad, *(new Idle(*this)), new Grunt::Agro(*this)) {
	movement.speed = 1.8;
	health = 5;
	type = "grunt";
	sprites.get("body")->setColor(255, 255, 0, 255);
	sprites.add("swipe", gd.swipe);
	sprites.get("swipe")->setDimensions(0.8, 1.6);
	sprites.get("swipe")->setColor(255, 255, 0, 255);
	b2Vec2 points1[]{
		b2Vec2(-gd.ad.size / 4, 0), b2Vec2(-gd.ad.size / 4, gd.ad.size * 2),
		b2Vec2(gd.ad.size / 4, gd.ad.size * 2), b2Vec2(gd.ad.size / 4, 0)
	};
	b2PolygonShape box;
	box.Set(points1, 4);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = true;
	fixtureDef.userData = (void*)'S';
	b2BodyPtr->CreateFixture(&fixtureDef);
}
Grunt::~Grunt() {
	
}
void Grunt::updateSprite() {
	int alpha;
	alpha = sprites.get("swipe")->canvas.color.a - 10;
	if (alpha < 0) { alpha = 0; }
	sprites.get("swipe")->setColor(255, 255, 255, alpha);
	Actor::updateSprite();
}
void Grunt::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else {
		if (myFixturePtr->GetUserData() != nullptr) {
			switch ((char)myFixturePtr->GetUserData()) {
			  case 'S':
				m_swipePtrs.insert(static_cast<Object*>(
					collisionFixturePtr->GetBody()->GetUserData())
				);
			  break;
			  default:
			  break;
			}
		}
	}
}
void Grunt::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else {
		if (myFixturePtr->GetUserData() != nullptr) {
			switch ((char)myFixturePtr->GetUserData()) {
			  case 'S':
				m_swipePtrs.erase(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  default:
			  break;
			}
		}
	}
}


void Grunt::Agro::enter() {
	actorPtr->sprites.get("body")->setColor(255, 255, 255, 255);
}
void Grunt::Agro::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	bool canSee = actorPtr->inLOS(actorPtrs[0], 4, "actor");
	if (!actorPtrs[0]->movement.dodging && canSee) {
		actorPtr->targetInfo.lastKnownPos = actorPtrs[0]->getPosition();
	}
	glm::vec2 targetVec = actorPtr->getPosition() - actorPtr->targetInfo.lastKnownPos;
	if (targetVec.x || targetVec.y) { actorPtr->los.faceDirection = glm::normalize(targetVec); }

	if (!actorPtr->getPathFindingData().staleData) {
		actorPtr->movement.vector = actorPtr->movement.speed * actorPtr->getNextPathPointDirection(
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
		actorPtr->movement.vector = glm::vec2(0);
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
	actorPtr->sprites.get("body")->setColor(255, 200, 200, 255);
}
void Grunt::Charge::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	bool canSee = actorPtr->inLOS(actorPtrs[0], 4, "actor");
	if (!actorPtrs[0]->movement.dodging) {
		actorPtr->targetInfo.lastKnownPos = actorPtrs[0]->getPosition();
	}
	glm::vec2 targetVec = actorPtr->getPosition() - actorPtr->targetInfo.lastKnownPos;
	if (targetVec.x || targetVec.y) { actorPtr->los.faceDirection = glm::normalize(targetVec); }

	if (!actorPtr->getPathFindingData().staleData) {
		actorPtr->movement.vector = actorPtr->movement.speed * actorPtr->getNextPathPointDirection(
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
		actorPtr->movement.vector = glm::vec2(0);
	}
	justEntered = false;

	//if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	if (!canSee) { actorPtr->setState(new Agro(*actorPtr)); }
	else if (glm::length(targetVec) < 0.8) {
		actorPtr->setState(new Attack(*actorPtr));
	}
}


void Grunt::Attack::enter() {
	actorPtr->sprites.get("body")->setColor(255, 100, 100, 255);
	actorPtr->sprites.get("swipe")->setColor(255, 0, 0, 255);
	actorPtr->sprites.get("swipe")->setPosition(
		actorPtr->b2BodyPtr->GetPosition().x, actorPtr->b2BodyPtr->GetPosition().y
	);
	actorPtr->sprites.get("swipe")->setRotationAxis(
		actorPtr->b2BodyPtr->GetPosition().x, actorPtr->b2BodyPtr->GetPosition().y
	);
	actorPtr->sprites.get("swipe")->canvas.rotationAngle = fk::makeAngle(
		actorPtr->los.faceDirection
	);
}
void Grunt::Attack::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	actorPtr->look(actorPtr->los.faceDirection);
	for (auto&& hitBodyPtr : static_cast<Grunt*>(actorPtr)->m_swipePtrs) {
		hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(-actorPtr->los.faceDirection.x * 10, -actorPtr->los.faceDirection.y * 10),
			hitBodyPtr->b2BodyPtr->GetPosition(),
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
