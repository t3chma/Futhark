#include "Grunt.h"
#include "Player.h"
#include <thread>

Grunt::Grunt(Map& map, ActorDef& ad) : Actor(map, ad) {
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
	p_speed = 1.8;
}
Grunt::~Grunt() {
	
}
void Grunt::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	if (hit) { p_pause += 10; }
	if (health < 1) { m_state = DEAD; }
	m_attacking = false;
	glm::vec2 position = getPosition();
	if (!actorPtrs[0]->isDodging()) { p_lastTargetPos = actorPtrs[0]->getPosition(); }
	glm::vec2 targetVec = getPosition() - p_lastTargetPos;
	float angle = std::abs(
		std::abs(fk::makeAngle(p_faceDirection) - fk::makeAngle(glm::normalize(targetVec))) - fk::TAU / 2
	);
	switch (m_state) {
	case RESTING:
		p_moveDirection.x = 0;
		p_moveDirection.y = 0;
		if (
			glm::length(targetVec) < 7 &&
			angle < fk::TAU / 4
		) {
			b2Vec2 self(getPosition().x, getPosition().y);
			b2Vec2 target(actorPtrs[0]->getPosition().x, actorPtrs[0]->getPosition().y);
			b2BodyPtr->GetWorld()->RayCast(this, self, target);
			if (p_obstructionPtr == actorPtrs[0]) { m_state = CASTING; }
		}
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
			if (m_counter++ > 50 && !m_rangen.getInt(0, 200)) {
				m_state = CHARGING;
				p_pathFindingData.staleData = true;
				m_canAttack = true;
				m_counter = 0;
			}
		} else {
			m_direction = m_rangen.getInt(0, 1);
			m_range = m_rangen.getInt(1, 3);
		}
		break;
	case CHARGING:
		if (targetVec.x || targetVec.y) { p_faceDirection = glm::normalize(targetVec); }
		p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
		if (m_counter > 30 && !p_pathFindingData.staleData) {
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
		if (glm::length(targetVec) < 0.8) { m_attacking = true; }
		if (m_attacking || hit || p_pause || m_counter > 6000) {
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
void Grunt::updateBody() {
	if (m_state != DEAD && !p_pause) {
		b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(p_moveDirection.x, p_moveDirection.y),
			b2BodyPtr->GetWorldCenter(),
			true
		);
		b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), p_faceAngle);
		if (m_attacking) {
			for (auto&& hitBodyPtr : m_hitPtrs) {
				hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
					b2Vec2(-p_faceDirection.x * 8, -p_faceDirection.y * 8),
					hitBodyPtr->b2BodyPtr->GetPosition(),
					true
				);
				static_cast<Object*>(hitBodyPtr)->health -= 1;
				static_cast<Object*>(hitBodyPtr)->hit = true;
			}
		}
	} else if (p_pause > 0) { --p_pause; }
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
		if (m_attacking) {
			spriteBatch[spriteIDs[1]].setColor(255, 255, 0, 255);
			spriteBatch[spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			spriteBatch[spriteIDs[1]].setRotationAxis(
				b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
			);
			spriteBatch[spriteIDs[2]].setColor(255, 255, 0, 255);
			spriteBatch[spriteIDs[2]].setPosition(
				b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
			);
			spriteBatch[spriteIDs[2]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = spriteBatch[spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[1]].setColor(255, 255, 0, alpha);
			alpha = spriteBatch[spriteIDs[2]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[2]].setColor(255, 255, 0, alpha);
		}
	} else {
		spriteBatch[spriteIDs[0]].setColor(255, 255, 0, 100);
		spriteBatch[spriteIDs[1]].setColor(255, 255, 0, 0);
		spriteBatch[spriteIDs[2]].setColor(255, 255, 0, 0);
	}
}