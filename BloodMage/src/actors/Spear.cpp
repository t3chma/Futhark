#include "Spear.h"


Spear::Spear(Map& map, ActorDef& ad) : Actor(map, ad) {
	spriteBatch[spriteIDs[0]].setColor(255, 0, 255, 255);
	spriteBatch[spriteIDs[1]].setDimensions(ad.size*0.5, ad.size*6);
	spriteBatch[spriteIDs[1]].setColor(255, 0, 255, 255);
	b2FixtureDef fixtureDef;
	b2PolygonShape box;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = true;
	b2Vec2 points1[]{
		b2Vec2(-ad.size / 4, 0), b2Vec2(-ad.size / 4, ad.size * 3),
		b2Vec2(ad.size / 4, ad.size * 3), b2Vec2(ad.size / 4, 0)
	};
	box.Set(points1, 4);
	fixtureDef.userData = (void*)'s';
	b2BodyPtr->CreateFixture(&fixtureDef);
	b2Vec2 points2[]{
		b2Vec2(-ad.size / 4, 0), b2Vec2(-ad.size / 4, ad.size * 1),
		b2Vec2(ad.size / 4, ad.size * 1), b2Vec2(ad.size / 4, 0)
	};
	box.Set(points2, 4);
	fixtureDef.userData = (void*)'r';
	b2BodyPtr->CreateFixture(&fixtureDef);
	type = "spear";
	health = 10;
	p_speed = 0.8;
}
Spear::~Spear() {
	
}
void Spear::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	if (health < 1) { m_state = DEAD; }
	glm::vec2 targetVec = glm::vec2(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y)
		- actorPtrs[0]->getPosition();
	int mod = 0;
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
			m_canAttack = true;
			m_state = CIRCLING;
			m_counter = 0;
		}
	  break;
	  case CIRCLING:
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (p_faceDirection.x || p_faceDirection.y) { p_moveDirection = p_speed * 1.5f * -p_faceDirection; }
		if (glm::length(targetVec) < m_range) {
			glm::vec2 oldMove = p_moveDirection;
			if (m_direction) { p_moveDirection = fk::rotatePoint(p_moveDirection, fk::TAU / 4); }
			else { p_moveDirection = fk::rotatePoint(p_moveDirection, -fk::TAU / 4); }
			if (m_counter++ > 40 && !m_rangen.getInt(0, 80)) {
				m_state = CHARGING;
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
		if (m_counter++ > 10) { p_moveDirection = p_speed * 1.0f * -p_faceDirection; }
		else { p_moveDirection = glm::vec2(0); }
		if (m_counter > 300) {
			m_state = CIRCLING;
			m_counter = 0;
		} else if (m_stabbing) {
			m_state = STABBING;
			m_counter = 0;
		}
	  break;
	  case STABBING:
		p_moveDirection = glm::vec2(0);
		if (m_counter <= 75) {
			mod = m_counter % 25;
			switch (mod) {
			  case 24:
				p_faceAngle = m_targetAngle;
				m_attacking = true;
			  break;
			  case 0:
				if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
				m_targetAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
				m_attacking = false;
			  break;
			  default:
			  break;
			}
		} else if (m_counter > 140) {
			m_state = CIRCLING;
			m_counter = 0;
			m_stabbing = false;
		}
		++m_counter;
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
void Spear::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else {
		if (myFixturePtr->GetUserData() != nullptr) {
			switch ((char)myFixturePtr->GetUserData()) {
			  case 's':
				m_hitPtrs.push_back(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  case 'r':
				m_rangePtrs.push_back(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
				if (
					m_rangePtrs.back()->category == "actor"
					&& m_rangePtrs.back()->type == "player"
					&& m_canAttack
				) { m_stabbing = true; }
			  break;
			  default:
			  break;
			}
		}
	}
}
void Spear::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else {
		if (myFixturePtr->GetUserData() != nullptr) {
			switch ((char)myFixturePtr->GetUserData()) {
			  case 's':
				m_hitPtrs.remove(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  case 'r':
				m_rangePtrs.remove(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
				m_stabbing = false;
			  break;
			  default:
			  break;
			}
		}
	}
}
void Spear::updateBody() {
	if (m_state != DEAD) {
		b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(p_moveDirection.x, p_moveDirection.y),
			b2BodyPtr->GetWorldCenter(),
			true
		);
		b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), p_faceAngle);
		if (m_attacking) {
			for (auto&& hitBodyPtr : m_hitPtrs) {
				hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
					b2Vec2(p_faceDirection.x * -4, p_faceDirection.y * -4),
					hitBodyPtr->b2BodyPtr->GetPosition(),
					true
				);
				static_cast<Object*>(hitBodyPtr)->health -= 1;
				static_cast<Object*>(hitBodyPtr)->hit = true;
			}
		}
	}
}
void Spear::updateSprite() {
	for (auto&& spriteID : spriteIDs) { spriteBatch[spriteID].canvas.rotationAngle = b2BodyPtr->GetAngle(); }
	spriteBatch[spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	if (m_state != DEAD) {
		if (m_state == CHARGING || m_state == STABBING) {
			spriteBatch[spriteIDs[0]].setColor(255, 0, 100, 255);
		} else {
			spriteBatch[spriteIDs[0]].setColor(255, 0, 255, 255);
		}
		if (m_attacking) {
			spriteBatch[spriteIDs[1]].setColor(255, 0, 255, 255);
			spriteBatch[spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			spriteBatch[spriteIDs[1]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = spriteBatch[spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[1]].setColor(255, 0, 255, alpha);
		}
	} else {
		spriteBatch[spriteIDs[0]].setColor(255, 0, 255, 100);
		spriteBatch[spriteIDs[1]].setColor(255, 0, 255, 0);
	}
}