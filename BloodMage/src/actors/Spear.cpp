#include "Spear.h"


Spear::Spear(fk::SpriteBatch* sbPtr, fk::World& world, fk::ActorDef& ad) : Actor(sbPtr, world, ad) {
	(*p_sbPtr)[p_spriteIDs[0]].setColor(0, 255, 0, 255);
	(*p_sbPtr)[p_spriteIDs[1]].setDimensions(ad.size*0.5, ad.size*10);
	(*p_sbPtr)[p_spriteIDs[1]].setColor(0, 255, 0, 255);
	b2FixtureDef fixtureDef;
	b2PolygonShape box;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = true;
	b2Vec2 points1[]{
		b2Vec2(-ad.size / 2, 0), b2Vec2(-ad.size / 2, -ad.size * 5),
		b2Vec2(ad.size / 2, -ad.size * 5), b2Vec2(ad.size / 2, 0)
	};
	box.Set(points1, 4);
	fixtureDef.userData = (void*)'s';
	b2BodyPtr->CreateFixture(&fixtureDef);
	b2Vec2 points2[]{
		b2Vec2(-ad.size / 2, 0), b2Vec2(-ad.size / 2, -ad.size * 2),
		b2Vec2(ad.size / 2, -ad.size * 2), b2Vec2(ad.size / 2, 0)
	};
	box.Set(points2, 4);
	fixtureDef.userData = (void*)'r';
	b2BodyPtr->CreateFixture(&fixtureDef);
	category = "Spear";
	p_health = 6;
	p_speed = 1;
}
Spear::~Spear() {
	
}
void Spear::think(std::vector<fk::Actor*>& actorPtrs, fk::Camera* camPtr) {
	if (p_health < 1) { m_state = DEAD; }
	glm::vec2 targetVec = actorPtrs[0]->getPosition() - glm::vec2(
		b2BodyPtr->GetPosition().x,
		b2BodyPtr->GetPosition().y
	);
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
		if (p_faceDirection.x || p_faceDirection.y) { p_moveDirection = p_speed * 1.5f * p_faceDirection; }
		if (glm::length(targetVec) < m_range) {
			glm::vec2 oldMove = p_moveDirection;
			if (m_direction) { p_moveDirection = fk::rotatePoint(p_moveDirection, fk::TAU / 4); }
			else { p_moveDirection = fk::rotatePoint(p_moveDirection, -fk::TAU / 4); }
			if (!m_rangen.getInt(0, 400)) {
				m_state = CHARGING;
			}
		} else {
			m_direction = m_rangen.getInt(0, 1);
			m_range = m_rangen.getInt(6, 10);
		}
	  break;
	  case CHARGING:
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (m_counter++ > 10) { p_moveDirection = p_speed * 1.0f * p_faceDirection; }
		else { p_moveDirection = glm::vec2(0); }
		if (m_stabbing || m_counter > 120) {
			m_state = STABBING;
			m_counter = 0;
		}
	  break;
	  case STABBING:
		p_moveDirection = glm::vec2(0);
		if (++m_counter <= 120 && m_counter >= 20) {
			mod = m_counter % 20;
			switch (mod) {
			  case 19:
				p_faceAngle = m_targetAngle;
				m_attacking = true;
			  break;
			  case 9:
				if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
				m_targetAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
			  break;
			  case 0:
				m_attacking = false;
			  break;
			  default:
			  break;
			}
		} else if (m_counter > 150) {
			m_state = CIRCLING;
			m_counter = 0;
			m_stabbing = false;
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
				m_hitPtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  case 'r':
				m_rangePtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
				if (m_rangePtrs.back()->category == "Player" && m_canAttack) { m_stabbing = true; }
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
				m_hitPtrs.clear();
			  break;
			  case 'r':
				m_rangePtrs.clear();
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
					b2Vec2(p_faceDirection.x * 4, p_faceDirection.y * 4),
					hitBodyPtr->b2BodyPtr->GetPosition(),
					true
				);
				hitBodyPtr->p_hit += 2;
			}
		}
	}
}
void Spear::updateSprite() {
	for (auto&& spriteID : p_spriteIDs) { (*p_sbPtr)[spriteID].canvas.rotationAngle = b2BodyPtr->GetAngle(); }
	(*p_sbPtr)[p_spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	if (m_state != DEAD) {
		if (m_attacking) {
			(*p_sbPtr)[p_spriteIDs[1]].setColor(0, 255, 0, 255);
			(*p_sbPtr)[p_spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			(*p_sbPtr)[p_spriteIDs[1]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = (*p_sbPtr)[p_spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			(*p_sbPtr)[p_spriteIDs[1]].setColor(0, 255, 0, alpha);
		}
	} else {
		(*p_sbPtr)[p_spriteIDs[0]].setColor(0, 255, 0, 100);
		(*p_sbPtr)[p_spriteIDs[1]].setColor(0, 255, 0, 0);
	}
}