#include "Grunt.h"
#include <thread>

Grunt::Grunt(Map& map, ActorDef& ad) : Actor(map, ad) {
	spriteBatch[spriteIDs[0]].setColor(255, 255, 0, 255);
	spriteBatch[spriteIDs[1]].setDimensions(1, 2);
	spriteBatch[spriteIDs[1]].setColor(255, 255, 0, 255);
	spriteBatch[spriteIDs[2]].setDimensions(1, 2);
	spriteBatch[spriteIDs[2]].setColor(255, 255, 0, 255);
	spriteBatch[spriteIDs[2]].setTextureDimensions(-1, 1);
	spriteBatch[spriteIDs[2]].setTexturePosition(1, 0);
	b2FixtureDef fixtureDef;
	b2PolygonShape box;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = true;
	b2Vec2 points1[]{
		b2Vec2(-ad.size / 4, 0), b2Vec2(-ad.size / 4, ad.size/2),
		b2Vec2(ad.size / 4, ad.size / 2), b2Vec2(ad.size / 4, 0)
	};
	box.Set(points1, 4);
	fixtureDef.userData = (void*)'l';
	b2BodyPtr->CreateFixture(&fixtureDef);
	b2Vec2 points2[]{
		b2Vec2(-ad.size / 4, 0), b2Vec2(-ad.size / 4, ad.size/2),
		b2Vec2(ad.size / 4, ad.size/2), b2Vec2(ad.size / 4, 0)
	};
	box.Set(points2, 4);
	fixtureDef.userData = (void*)'r';
	b2BodyPtr->CreateFixture(&fixtureDef);
	type = "grunt";
	health = 10;
	p_speed = 2;
}
Grunt::~Grunt() {
	
}
void Grunt::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	if (health < 1) { m_state = DEAD; }
	glm::vec2 position = getPosition();
	glm::vec2 targetVec = glm::vec2(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y)
		- actorPtrs[0]->getPosition();
	switch (m_state) {
	  case RESTING:
		p_moveDirection.x = 0;
		p_moveDirection.y = 0;
		if (glm::length(targetVec) < 15) { m_state = CASTING; }
	  break;
	  case CASTING:
		p_moveDirection.x = 0;
		p_moveDirection.y = 0;
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (m_counter++ > 120) {
			m_counter = 0;
			m_state = CIRCLING;
			m_counter = 0;
		}
	  break;
	  case CIRCLING:
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (p_faceDirection.x || p_faceDirection.y) { p_moveDirection = p_speed * -p_faceDirection; }
		if (glm::length(targetVec) < m_range) {
			if (m_direction) { p_moveDirection = fk::rotatePoint(p_moveDirection, fk::TAU / 4); }
			else { p_moveDirection = fk::rotatePoint(p_moveDirection, -fk::TAU / 4); }
			if (m_counter++ > 40 && !m_rangen.getInt(0, 80)) {
				m_state = CHARGING;
				p_pathFindingData.staleData = true;
				m_canAttack = true;
				m_counter = 0;
			}
		} else {
			m_direction = m_rangen.getInt(0, 1);
			m_range = m_rangen.getInt(1, 4);
		}
	  break;
	  case CHARGING:
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (m_counter > 10 && !p_pathFindingData.staleData) {
			glm::vec2 pos(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			pos.x = round(pos.x);
			pos.y = round(pos.y);
			if (p_pathFindingData.path.size() > 1 && pos == p_pathFindingData.path.front()) {
				p_pathFindingData.path.pop_front();
			}
			glm::vec2 targ = p_pathFindingData.path.front();
			glm::vec2 moveVec = targ - pos;
			if (moveVec.x == 0 && moveVec.y == 0 || glm::length(moveVec) > 2) {
				moveVec = actorPtrs[0]->getPosition() - pos;
			}
			p_moveDirection = p_speed * 1.0f * glm::normalize(moveVec);
			if (p_pathFindingData.upToDate) { startAStar(actorPtrs[0]->getPosition()); }
			else { advanceAStar(); }
		} else {
			if (m_counter == 0) { startAStar(actorPtrs[0]->getPosition()); }
			else { advanceAStar(); }
			p_moveDirection = glm::vec2(0);
		}
		++m_counter;
		if (m_attackingLeft || m_attackingRight || hit || m_counter > 6000) {
			m_state = CIRCLING;
			m_canAttack = false;
			m_counter = 0;
		}
	  break;
	  case RETREATING:

	  break;
	  case DEAD:
		p_moveDirection.x = 0;
		p_moveDirection.y = 0;
	  break;
	}
	hit = false;
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
				m_leftHitPtrs.push_back(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
				if (
					m_leftHitPtrs.back()->category == "actor"
					&& m_rightHitPtrs.back()->type == "player"
					&& m_canAttack
				) {
					m_attackingLeft = true;
				}
			  break;
			  case 'r':
				m_rightHitPtrs.push_back(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
				if (
					m_rightHitPtrs.back()->category == "actor"
					&& m_rightHitPtrs.back()->type == "player"
					&& m_canAttack
				) {
					m_attackingRight = true;
				}
			  break;
			  default:
			  break;
			}
		} else {
			m_swipeRangePtrs.push_back(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
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
				m_leftHitPtrs.remove(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
				m_attackingLeft = false;
			  break;
			  case 'r':
				m_rightHitPtrs.remove(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
				m_attackingRight = false;
			  break;
			  default:
			  break;
			}
		} else {
			m_swipeRangePtrs.remove(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
		}
	}
}
void Grunt::updateBody() {
	if (m_state != DEAD) {
		if (test++ % 2 == 0) {
		b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(p_moveDirection.x * 2, p_moveDirection.y*2),
			b2BodyPtr->GetWorldCenter(),
			true
		);
		}
		b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), p_faceAngle);
		if (!m_swipeRangePtrs.empty()) {
			if (m_attackingLeft) {
				for (auto&& hitBodyPtr : m_leftHitPtrs) {
					hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
						b2Vec2(-p_faceDirection.x, -p_faceDirection.y),
						hitBodyPtr->b2BodyPtr->GetPosition(),
						true
					);
					static_cast<Object*>(hitBodyPtr)->health -= 1;
					static_cast<Object*>(hitBodyPtr)->hit = true;
				}
			}
			if (m_attackingLeft) {
				for (auto&& hitBodyPtr : m_rightHitPtrs) {
					hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
						b2Vec2(-p_faceDirection.x, -p_faceDirection.y),
						hitBodyPtr->b2BodyPtr->GetPosition(),
						true
					);
					static_cast<Object*>(hitBodyPtr)->health -= 1;
					static_cast<Object*>(hitBodyPtr)->hit = true;
				}
			}
		}
	}
}
void Grunt::updateSprite() {
	for (auto&& spriteID : spriteIDs) { spriteBatch[spriteID].canvas.rotationAngle = b2BodyPtr->GetAngle(); }
	spriteBatch[spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	if (m_state != DEAD) {
		if (m_state == CHARGING) {
			spriteBatch[spriteIDs[0]].setColor(255, 100, 0, 255);
		} else {
			spriteBatch[spriteIDs[0]].setColor(255, 255, 0, 255);
		}
		if (m_attackingRight) {
			spriteBatch[spriteIDs[1]].setColor(255, 255, 0, 255);
			spriteBatch[spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			spriteBatch[spriteIDs[1]].setRotationAxis(
				b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
			);
		} else {
			int alpha = spriteBatch[spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[1]].setColor(255, 255, 0, alpha);
		}
		if (m_attackingLeft) {
			spriteBatch[spriteIDs[2]].setColor(255, 255, 0, 255);
			spriteBatch[spriteIDs[2]].setPosition(
				b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
			);
			spriteBatch[spriteIDs[2]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = spriteBatch[spriteIDs[2]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[2]].setColor(255, 255, 0, alpha);
		}
	} else {
		spriteBatch[spriteIDs[0]].setColor(255, 255, 0, 100);
		spriteBatch[spriteIDs[1]].setColor(255, 255, 0, 0);
		spriteBatch[spriteIDs[2]].setColor(255, 255, 0, 0);
	}
}