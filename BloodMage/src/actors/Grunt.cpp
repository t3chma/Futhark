#include "Grunt.h"


Grunt::Grunt(fk::SpriteBatch* sbPtr, fk::World& world, ActorDef& ad) : Actor(sbPtr, world, ad) {
	(*p_sbPtr)[p_spriteIDs[0]].setColor(255, 255, 0, 255);
	(*p_sbPtr)[p_spriteIDs[1]].setDimensions(1, 2);
	(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 0, 255);
	(*p_sbPtr)[p_spriteIDs[2]].setDimensions(1, 2);
	(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 0, 255);
	(*p_sbPtr)[p_spriteIDs[2]].setTextureDimensions(-1, 1);
	(*p_sbPtr)[p_spriteIDs[2]].setTexturePosition(1, 0);
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
	category = "Grunt";
	p_health = 10;
	p_speed = 1;
}
Grunt::~Grunt() {
	
}
void Grunt::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	for (int i = 0; i < 1000; ++i) {
		test += m_counter;
	}
	if (p_health < 1) { m_state = DEAD; }
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
			glm::vec2 oldMove = p_moveDirection;
			if (m_direction) { p_moveDirection = fk::rotatePoint(p_moveDirection, fk::TAU / 4); }
			else { p_moveDirection = fk::rotatePoint(p_moveDirection, -fk::TAU / 4); }
			if (m_counter++ > 40 && !m_rangen.getInt(0, 80)) {
				m_state = CHARGING;
				m_canAttack = true;
				m_counter = 0;
			}
		} else {
			m_direction = m_rangen.getInt(0, 1);
			m_range = m_rangen.getFloat(3, 5);
		}
	  break;
	  case CHARGING:
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (m_counter++ > 10) { p_moveDirection = p_speed * 1.1f * -p_faceDirection; }
		else { p_moveDirection = glm::vec2(0); }
		if (m_attackingLeft || m_attackingRight || p_hit || m_counter > 200) {
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
	if (p_hit) { p_health -= p_hit; p_hit = 0; }
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
				m_leftHitPtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
				if (m_leftHitPtrs.back()->category == "Player" && m_canAttack) { m_attackingLeft = true; }
			  break;
			  case 'r':
				m_rightHitPtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
				if (m_rightHitPtrs.back()->category == "Player" && m_canAttack) { m_attackingRight = true; }
			  break;
			  default:
			  break;
			}
		} else {
			m_swipeRangePtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
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
				m_leftHitPtrs.remove(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
				m_attackingLeft = false;
			  break;
			  case 'r':
				m_rightHitPtrs.remove(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
				m_attackingRight = false;
			  break;
			  default:
			  break;
			}
		} else {
			m_swipeRangePtrs.remove(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
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
						b2Vec2(p_faceDirection.x * -2, p_faceDirection.y * -2),
						hitBodyPtr->b2BodyPtr->GetPosition(),
						true
					);
					++hitBodyPtr->p_hit;
				}
			}
			if (m_attackingLeft) {
				for (auto&& hitBodyPtr : m_rightHitPtrs) {
					hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
						b2Vec2(p_faceDirection.x * -2, p_faceDirection.y * -2),
						hitBodyPtr->b2BodyPtr->GetPosition(),
						true
					);
					++hitBodyPtr->p_hit;
				}
			}
		}
	}
}
void Grunt::updateSprite() {
	for (auto&& spriteID : p_spriteIDs) { (*p_sbPtr)[spriteID].canvas.rotationAngle = b2BodyPtr->GetAngle(); }
	(*p_sbPtr)[p_spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	if (m_state != DEAD) {
		if (m_state == CHARGING) {
			(*p_sbPtr)[p_spriteIDs[0]].setColor(255, 100, 0, 255);
		} else {
			(*p_sbPtr)[p_spriteIDs[0]].setColor(255, 255, 0, 255);
		}
		if (m_attackingRight) {
			(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 0, 255);
			(*p_sbPtr)[p_spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			(*p_sbPtr)[p_spriteIDs[1]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = (*p_sbPtr)[p_spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 0, alpha);
		}
		if (m_attackingLeft) {
			(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 0, 255);
			(*p_sbPtr)[p_spriteIDs[2]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			(*p_sbPtr)[p_spriteIDs[2]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = (*p_sbPtr)[p_spriteIDs[2]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 0, alpha);
		}
	} else {
		(*p_sbPtr)[p_spriteIDs[0]].setColor(255, 255, 0, 100);
		(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 0, 0);
		(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 0, 0);
	}
}