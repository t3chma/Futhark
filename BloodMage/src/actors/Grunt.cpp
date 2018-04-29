#include "Grunt.h"
#include "Player.h"
#include <thread>


Grunt::Grunt(Map& map, ActorDef& ad) : Actor(map, ad, *(new Idle(*this)), GRUNT) {
	spriteBatch[spriteIDs[0]].setColor(255, 255, 0, 255);
	spriteBatch[spriteIDs[1]].setDimensions(0.8, 1.6);
	spriteBatch[spriteIDs[1]].setColor(255, 255, 0, 255);
	spriteBatch[spriteIDs[2]].setDimensions(0.8, 1.6);
	spriteBatch[spriteIDs[2]].setColor(255, 255, 0, 255);
	spriteBatch[spriteIDs[2]].setTextureDimensions(-1, 1);
	spriteBatch[spriteIDs[2]].setTexturePosition(1, 0);
	b2FixtureDef fixtureDef;
	b2PolygonShape box;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = true;
	b2Vec2 points1[]{
		b2Vec2(-ad.size / 4, 0), b2Vec2(-ad.size / 4, ad.size * 2),
		b2Vec2(ad.size / 4, ad.size * 2), b2Vec2(ad.size / 4, 0)
	};
	box.Set(points1, 4);
	fixtureDef.userData = (void*)'l';
	b2BodyPtr->CreateFixture(&fixtureDef);
	type = "grunt";
	health = 5;
	movement.speed = 1.8;
}
Grunt::~Grunt() {
	
}
void Grunt::updateSprite() {
	int alpha;
	for (int i = 1; i < spriteIDs.size(); ++i) {
		alpha = spriteBatch[spriteIDs[i]].canvas.color.a - 10;
		if (alpha < 0) { alpha = 0; }
		spriteBatch[spriteIDs[i]].setColor(255, 255, 255, alpha);
	}
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
			  case 'l':
				m_hitPtrs.push_back(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
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
			  case 'l':
				m_hitPtrs.remove(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  default:
			  break;
			}
		}
	}
}


void Grunt::Agro::enter() {
	actorPtr->spriteBatch[actorPtr->spriteIDs[0]].setColor(255, 255, 255, 255);
}
void Grunt::Agro::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	bool canSee = actorPtr->inLOS(actorPtrs[0], 4, "actor");
	if (!actorPtrs[0]->movement.dodging && canSee) {
		actorPtr->targetInfo.lastPos = actorPtrs[0]->getPosition();
	}
	glm::vec2 targetVec = actorPtr->getPosition() - actorPtr->targetInfo.lastPos;
	if (targetVec.x || targetVec.y) { actorPtr->los.faceDirection = glm::normalize(targetVec); }

	if (!actorPtr->getPathFindingData().staleData) {
		actorPtr->movement.vector = actorPtr->movement.speed * actorPtr->getNextPathPointDirection(
			actorPtr->targetInfo.lastPos
		);
		if (actorPtr->getPathFindingData().done) {
			float length = glm::length(targetVec);
			if (!canSee || length > range) {
				actorPtr->startAStar(actorPtr->targetInfo.lastPos);
			} else {
				glm::vec2 targ(0);
				int division = 5;
				bool redo(false);
				do {
					float cost(0);
					// Rotate
					targ = fk::rotatePoint(
						actorPtr->targetInfo.lastPos - actorPtr->getPosition(), fk::TAU / division * direction
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
		if (actorPtr->getPathFindingData().done) { actorPtr->startAStar(actorPtr->targetInfo.lastPos); }
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
	actorPtr->spriteBatch[actorPtr->spriteIDs[0]].setColor(255, 200, 200, 255);
}
void Grunt::Charge::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	bool canSee = actorPtr->inLOS(actorPtrs[0], 4, "actor");
	if (!actorPtrs[0]->movement.dodging) { actorPtr->targetInfo.lastPos = actorPtrs[0]->getPosition(); }
	glm::vec2 targetVec = actorPtr->getPosition() - actorPtr->targetInfo.lastPos;
	if (targetVec.x || targetVec.y) { actorPtr->los.faceDirection = glm::normalize(targetVec); }

	if (!actorPtr->getPathFindingData().staleData) {
		actorPtr->movement.vector = actorPtr->movement.speed * actorPtr->getNextPathPointDirection(
			actorPtr->targetInfo.lastPos
		);
		if (actorPtr->getPathFindingData().done) { actorPtr->startAStar(actorPtr->targetInfo.lastPos); }
		else { actorPtr->advanceAStar(); }
	} else {
		if (actorPtr->getPathFindingData().done) { actorPtr->startAStar(actorPtr->targetInfo.lastPos); }
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
	actorPtr->spriteBatch[actorPtr->spriteIDs[0]].setColor(255, 100, 100, 255);
	actorPtr->spriteBatch[actorPtr->spriteIDs[1]].setColor(255, 0, 0, 255);
	actorPtr->spriteBatch[actorPtr->spriteIDs[1]].setPosition(
		actorPtr->b2BodyPtr->GetPosition().x, actorPtr->b2BodyPtr->GetPosition().y
	);
	actorPtr->spriteBatch[actorPtr->spriteIDs[1]].setRotationAxis(
		actorPtr->b2BodyPtr->GetPosition().x, actorPtr->b2BodyPtr->GetPosition().y
	);
	actorPtr->spriteBatch[actorPtr->spriteIDs[1]].canvas.rotationAngle = fk::makeAngle(
		actorPtr->los.faceDirection
	);
	actorPtr->spriteBatch[actorPtr->spriteIDs[2]].setColor(255, 0, 0, 255);
	actorPtr->spriteBatch[actorPtr->spriteIDs[2]].setPosition(
		actorPtr->b2BodyPtr->GetPosition().x, actorPtr->b2BodyPtr->GetPosition().y
	);
	actorPtr->spriteBatch[actorPtr->spriteIDs[2]].setRotationAxis(
		actorPtr->b2BodyPtr->GetPosition().x, actorPtr->b2BodyPtr->GetPosition().y
	);
	actorPtr->spriteBatch[actorPtr->spriteIDs[2]].canvas.rotationAngle = fk::makeAngle(
		actorPtr->los.faceDirection
	);
}
void Grunt::Attack::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	actorPtr->look(actorPtr->los.faceDirection);
	for (auto&& hitBodyPtr : static_cast<Grunt*>(actorPtr)->m_hitPtrs) {
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
