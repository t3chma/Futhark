#include "Player.h"
#include "base/Utility.h"

Player::Player(
	Map& map,
	fk::UserInput* uiPtr,
	ActorDef& ad
) : Actor(map, ad), m_uiPtr(uiPtr) {
	spriteBatch[spriteIDs[1]].setDimensions(ad.size*2, ad.size*3);
	spriteBatch[spriteIDs[1]].setColor(255, 255, 255, 0);
	spriteBatch[spriteIDs[2]].setDimensions(ad.size*2, ad.size*3);
	spriteBatch[spriteIDs[2]].setColor(255, 255, 255, 0);
	spriteBatch[spriteIDs[2]].setTextureDimensions(-1, 1);
	spriteBatch[spriteIDs[2]].setTexturePosition(1, 0);
	spriteBatch[spriteIDs[3]].setDimensions(ad.size, ad.size * 32);
	spriteBatch[spriteIDs[3]].setColor(255, 255, 255, 0);
	spriteBatch[spriteIDs[4]].setDimensions(ad.size, ad.size * 32);
	spriteBatch[spriteIDs[4]].setColor(255, 255, 255, 0);
	spriteBatch[spriteIDs[4]].setTextureDimensions(-1, 1);
	spriteBatch[spriteIDs[4]].setTexturePosition(1, 0);
	type = "player";
	health = 50;
}
Player::~Player() {

}
void Player::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).position);
	p_speed = 1;
	if (!m_dodge) {
		p_moveDirection.x = 0;
		p_moveDirection.y = 0;
	}
	if (m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames > 1) {
		
	} else {
		if (m_uiPtr->getKeyInfo(fk::Key::W).downFrames > 1) {
			p_moveDirection.y += 1;
			if (m_moveKeys.w > 0 && m_moveKeys.w < 10 && !m_dodgeTimer) {
				m_dodgePos = getPosition(); m_dodge = 10; m_targetPtr = nullptr;
			}
			m_moveKeys.w = 0;
		}
		else { ++m_moveKeys.w; }
		if (m_uiPtr->getKeyInfo(fk::Key::S).downFrames > 1) {
			p_moveDirection.y -= 1;
			if (m_moveKeys.s > 0 && m_moveKeys.s < 10 && !m_dodgeTimer) {
				m_dodgePos = getPosition(); m_dodge = 10; m_targetPtr = nullptr;
			}
			m_moveKeys.s = 0;
		}
		else { ++m_moveKeys.s; }
		if (m_uiPtr->getKeyInfo(fk::Key::D).downFrames > 1) {
			p_moveDirection.x += 1;
			if (m_moveKeys.d > 0 && m_moveKeys.d < 10 && !m_dodgeTimer) {
				m_dodgePos = getPosition(); m_dodge = 10; m_targetPtr = nullptr;
			}
			m_moveKeys.d = 0;
		}
		else { ++m_moveKeys.d; }
		if (m_uiPtr->getKeyInfo(fk::Key::A).downFrames > 1) {
			p_moveDirection.x -= 1;
			if (m_moveKeys.a > 0 && m_moveKeys.a < 10 && !m_dodgeTimer) {
				m_dodgePos = getPosition(); m_dodge = 10; m_targetPtr = nullptr;
			}
			m_moveKeys.a = 0;
		}
		else { ++m_moveKeys.a; }
		if (m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames > 1) { p_speed = 1.75; m_dodge = 0; }
	}
	if (m_leftSwipe > 0) { --m_leftSwipe; }
	if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1) {
		m_getTarget = true;
		if (m_leftSwipe == 0) { m_leftSwipe = 30; }
	}
	if (m_rightSwipe > 0) { --m_rightSwipe; }
	if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1) {
		m_getTarget = true;
		if (m_rightSwipe == 0) { m_rightSwipe = 30; }
	}
	glm::vec2 position = glm::vec2(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	if (m_targetPtr) { p_faceDirection = glm::normalize(position - m_targetPtr->getPosition()); }
	else if (m_mousePos != position) { p_faceDirection = glm::normalize(position - m_mousePos); }
	p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
	if (p_moveDirection.x || p_moveDirection.y) { p_moveDirection = p_speed * glm::normalize(p_moveDirection); }
	hit = false;
}
void Player::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (!collisionFixturePtr->IsSensor()) {
		if (m_dodge) {
			Object* op = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
			if (op->category == "actor") {
				static_cast<Actor*>(op)->pause(30);
			}
		}
	}
}
void Player::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (!collisionFixturePtr->IsSensor()) {}
}
void Player::updateBody() {
	if (health > 0) {
		if (m_dodge) { b2BodyPtr->GetFixtureList()[0].SetSensor(true); }
		else { b2BodyPtr->GetFixtureList()[0].SetSensor(false); }
		b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), p_faceAngle);
		if (m_getTarget) {
			m_getTarget = false;
			m_targetPtr = nullptr;
			if (
				glm::length(getPosition() - m_mousePos) < b2BodyPtr->GetFixtureList()[0].GetShape()->m_radius
				&& (m_leftSwipe == 30 || m_rightSwipe == 30)
			) {

			}
			b2BodyPtr->GetWorld()->RayCast(
				this,
				b2Vec2(getPosition().x, getPosition().y),
				b2Vec2(m_mousePos.x, m_mousePos.y)
			);
			if (m_targetPtr == nullptr) { m_leftSwipe = m_rightSwipe = 0; }
		}
		bool move{ true };
		if (m_targetPtr && (m_leftSwipe || m_rightSwipe)) {
			glm::vec2 targetDirection = getPosition() - m_targetPtr->getPosition();
			float distance = glm::length(getPosition() - m_targetPtr->getPosition());
			if (distance < 3) {
				m_targetPtr->pause(60);
				if (distance > 0.7) {
					b2BodyPtr->ApplyLinearImpulse(
						b2Vec2(-targetDirection.x * 2, -targetDirection.y * 2),
						b2BodyPtr->GetWorldCenter(),
						true
					);
					if (m_leftSwipe == 1) { m_leftSwipe = 2; }
					if (m_rightSwipe == 1) { m_rightSwipe = 2; }
					move = false;
				} else {
					if (m_leftSwipe == 1) {
						if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames > 10) {
							spriteBatch[spriteIDs[1]].setPosition(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[1]].setRotationAxis(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[1]].canvas.rotationAngle = b2BodyPtr->GetAngle() + fk::TAU / 8;
							spriteBatch[spriteIDs[1]].setColor(255, 255, 255, 255);
							m_leftSwipe = 20;
						} else {
							spriteBatch[spriteIDs[3]].setPosition(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[3]].setRotationAxis(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[3]].canvas.rotationAngle = b2BodyPtr->GetAngle() + fk::TAU / 8;
							spriteBatch[spriteIDs[3]].setColor(255, 255, 255, 255);
							m_targetPtr->b2BodyPtr->ApplyLinearImpulse(
								b2Vec2(p_faceDirection.x * -3, p_faceDirection.y * -3),
								m_targetPtr->b2BodyPtr->GetPosition(),
								true
							);
							m_targetPtr->health -= 1;
						}
						m_targetPtr->hit = true;
					}
					if (m_rightSwipe == 1) {
						if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames > 10) {
							spriteBatch[spriteIDs[2]].setPosition(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[2]].setRotationAxis(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[2]].canvas.rotationAngle = b2BodyPtr->GetAngle() - fk::TAU / 8;
							spriteBatch[spriteIDs[2]].setColor(255, 255, 255, 255);
							m_rightSwipe = 20;
						}
						else {
							spriteBatch[spriteIDs[4]].setPosition(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[4]].setRotationAxis(
								m_targetPtr->b2BodyPtr->GetPosition().x, m_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[4]].canvas.rotationAngle = b2BodyPtr->GetAngle() - fk::TAU / 8;
							spriteBatch[spriteIDs[4]].setColor(255, 255, 255, 255);
							m_targetPtr->b2BodyPtr->ApplyLinearImpulse(
								b2Vec2(p_faceDirection.x * -3, p_faceDirection.y * -3),
								m_targetPtr->b2BodyPtr->GetPosition(),
								true
							);
							m_targetPtr->health -= 1;
						}
						m_targetPtr->hit = true;
					}
				}
			} else { m_targetPtr = nullptr; }
		}
		if (move) {
			float dodgeMult = 1;
			if (m_dodge == 10) { m_dodgeTimer = 32; }
			if (m_dodge > 7) { dodgeMult = 20; }
			b2BodyPtr->ApplyLinearImpulse(
				b2Vec2(p_moveDirection.x * dodgeMult, p_moveDirection.y * dodgeMult),
				b2BodyPtr->GetWorldCenter(),
				true
			);
		}
		if (m_dodge) { --m_dodge; }
		if (m_dodgeTimer) { --m_dodgeTimer; }
	}
}
void Player::updateSprite() {
	spriteBatch[spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[1]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[2]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	if (health > 0) {
		if (m_rightSwipe == 60) {
			spriteBatch[spriteIDs[1]].setColor(255, 255, 255, 255);
			spriteBatch[spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			spriteBatch[spriteIDs[1]].setRotationAxis(
				b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
			);
		} else {
			int alpha = spriteBatch[spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[1]].setColor(255, 255, 255, alpha);
			alpha = spriteBatch[spriteIDs[3]].canvas.color.a - 10;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[3]].setColor(255, 255, 255, alpha);
		}
		if (m_leftSwipe == 60) {
			spriteBatch[spriteIDs[2]].setColor(255, 255, 255, 255);
			spriteBatch[spriteIDs[2]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			spriteBatch[spriteIDs[2]].setRotationAxis(
				b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
			);
		} else {
			int alpha = spriteBatch[spriteIDs[2]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[2]].setColor(255, 255, 255, alpha);
			alpha = spriteBatch[spriteIDs[4]].canvas.color.a - 10;
			if (alpha < 0) { alpha = 0; }
			spriteBatch[spriteIDs[4]].setColor(255, 255, 255, alpha);
		}
	} else {
		spriteBatch[spriteIDs[0]].setColor(255, 255, 255, 100);
		spriteBatch[spriteIDs[1]].setColor(255, 255, 255, 0);
		spriteBatch[spriteIDs[2]].setColor(255, 255, 255, 0);
		spriteBatch[spriteIDs[3]].setColor(255, 255, 255, 0);
		spriteBatch[spriteIDs[4]].setColor(255, 255, 255, 0);
	}
}

float32 Player::ReportFixture(b2Fixture* fixturePtr, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
	if (static_cast<Object*>(fixturePtr->GetBody()->GetUserData())->category == "actor") {
		m_targetPtr = static_cast<Actor*>(fixturePtr->GetBody()->GetUserData());
	}
	return fraction;
}

glm::vec2 Player::getPosition() {
	if (m_dodge) { return m_dodgePos; }
	else {
		b2Vec2 vec = b2BodyPtr->GetPosition();
		return glm::vec2(vec.x, vec.y);
	}
}
