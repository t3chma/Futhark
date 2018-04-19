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
	b2FixtureDef fixtureDef;
	b2CircleShape circle;
	circle.m_radius = ad.size * 2;
	fixtureDef.shape = &circle;
	fixtureDef.isSensor = true;
	fixtureDef.userData = (void*)'r';
	b2BodyPtr->CreateFixture(&fixtureDef);
}
Player::~Player() {

}
void Player::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).position);
	glm::vec2 position = glm::vec2(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	p_speed = 1;
	if (m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames > 0) { p_speed = 1.75; p_targetPtr = nullptr; }
	else {
		// Left attack
		if (m_leftSwipe > 0) { --m_leftSwipe; }
		if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1) {
			m_getTarget = true;
			if (m_leftSwipe == 0) { m_leftSwipe = 30; }
		}
		// Right attack
		if (m_rightSwipe > 0) { --m_rightSwipe; }
		if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1) {
			m_getTarget = true;
			if (m_rightSwipe == 0) { m_rightSwipe = 30; }
		}
	}
	// Face direction
	if (m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames > 0) { p_targetPtr = nullptr; }
	if (p_targetPtr && position != p_targetPtr->getPosition()) {
		p_faceDirection = glm::normalize(position - p_targetPtr->getPosition());
	} else if (m_mousePos != position) {
		p_faceDirection = glm::normalize(position - m_mousePos);
	}
	p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU / 4;
	// Move direction
	if (!p_dodging) {
		p_moveDirection.x = 0;
		p_moveDirection.y = 0;
	}
	if (m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames > 1) {

	} else if (!p_dodging) {
		// WASD
		if (m_uiPtr->getKeyInfo(fk::Key::W).downFrames > 0) { p_moveDirection.y += 1; }
		if (m_uiPtr->getKeyInfo(fk::Key::S).downFrames > 0) { p_moveDirection.y -= 1; }
		if (m_uiPtr->getKeyInfo(fk::Key::D).downFrames > 0) { p_moveDirection.x += 1; }
		if (m_uiPtr->getKeyInfo(fk::Key::A).downFrames > 0) { p_moveDirection.x -= 1; }
		// Dodge
		if (p_speed == 1) {
			if (m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames > 0) {
				p_moveDirection.x = 0;
				p_moveDirection.y = 0;
				++m_dodgeCharge;
			} else if (m_dodgeCharge && !m_dodgeTimer) {
				m_dodgeTimer = 40;
				m_dodgePos = getPosition();
				p_dodging = m_dodgeIFrames;
				p_targetPtr = nullptr;
				if (m_dodgeCharge > 20) { p_moveDirection = -p_faceDirection; }
				else { p_moveDirection = p_faceDirection; }
			}
		}
	}
	if (p_moveDirection.x || p_moveDirection.y) { p_moveDirection = p_speed * glm::normalize(p_moveDirection); }
	if (m_dodgeCharge > 20) { p_moveDirection *= 1.5; }
	hit = false;
}
void Player::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (!collisionFixturePtr->IsSensor()) {
		if (p_dodging) {
			Object* op = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
			if (op->category == "actor" && m_dodgeCharge > 20) {
				static_cast<Actor*>(op)->pause(60);
				static_cast<Actor*>(op)->hit = true;
			} else if (op->category == "static" && !myFixturePtr->GetUserData()) {
				p_dodging = 0; m_dodgeCharge = 0;
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
		b2Filter filter = b2BodyPtr->GetFixtureList()->GetNext()->GetFilterData();
		if (p_dodging) { filter.maskBits = 1; }
		else { filter.maskBits = 0b1111111111111111; }
		b2BodyPtr->GetFixtureList()->GetNext()->SetFilterData(filter);
		b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), p_faceAngle);
		if (m_getTarget) {
			m_getTarget = false;
			p_targetPtr = nullptr;
			glm::vec2 mp = glm::normalize(m_mousePos - getPosition());
			mp *= 3;
			glm::vec2 mpl = fk::rotatePoint(mp, fk::TAU/25);
			glm::vec2 mpr = fk::rotatePoint(mp, -fk::TAU/25);;
			b2BodyPtr->GetWorld()->RayCast(
				this,
				b2Vec2(getPosition().x, getPosition().y),
				b2Vec2(getPosition().x + mp.x, getPosition().y + mp.y)
			);
			b2BodyPtr->GetWorld()->RayCast(
				this,
				b2Vec2(getPosition().x, getPosition().y),
				b2Vec2(getPosition().x + mpl.x, getPosition().y + mpl.y)
			);
			b2BodyPtr->GetWorld()->RayCast(
				this,
				b2Vec2(getPosition().x, getPosition().y),
				b2Vec2(getPosition().x + mpr.x, getPosition().y + mpr.y)
			);
			if (p_targetPtr == nullptr) { m_leftSwipe = m_rightSwipe = 0; }
		}
		bool move{ true };
		m_charging = false;
		if (p_targetPtr && (m_leftSwipe || m_rightSwipe)) {
			glm::vec2 targetDirection = getPosition() - p_targetPtr->getPosition();
			float distance = glm::length(getPosition() - p_targetPtr->getPosition());
			if (distance < 2) {
				if (distance >= 0.6) {
					if (
						m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames ||
						m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
					) {
						m_charging = true;
						b2BodyPtr->ApplyLinearImpulse(
							b2Vec2(-targetDirection.x * 3, -targetDirection.y * 3),
							b2BodyPtr->GetWorldCenter(),
							true
						);
						if (m_leftSwipe == 1) { m_leftSwipe = 2; }
						if (m_rightSwipe == 1) { m_rightSwipe = 2; }
						move = false;
					}
				}
				if (distance <= 2) {
					p_targetPtr->pause(30);
					m_charging = true;
					if (m_leftSwipe == 1) {
						if (p_drainCount.left < 4 && m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames > 10) {
							spriteBatch[spriteIDs[1]].setPosition(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[1]].setRotationAxis(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[1]].canvas.rotationAngle = b2BodyPtr->GetAngle() + fk::TAU / 8;
							spriteBatch[spriteIDs[1]].setColor(255, 255, 255, 255);
							m_leftSwipe = 15;
							++p_drainCount.left;
						} else {
							spriteBatch[spriteIDs[3]].setPosition(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[3]].setRotationAxis(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[3]].canvas.rotationAngle = b2BodyPtr->GetAngle() + fk::TAU / 8;
							spriteBatch[spriteIDs[3]].setColor(255, 255, 255, 255);
							p_targetPtr->b2BodyPtr->ApplyLinearImpulse(
								b2Vec2(p_faceDirection.x * -7, p_faceDirection.y * -7),
								p_targetPtr->b2BodyPtr->GetPosition(),
								true
							);
							p_targetPtr->health -= 1;
							p_drainCount.left = 0;
						}
						p_targetPtr->hit = true;
					}
					if (m_rightSwipe == 1) {
						if (p_drainCount.right < 4 && m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames > 10) {
							spriteBatch[spriteIDs[2]].setPosition(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[2]].setRotationAxis(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[2]].canvas.rotationAngle = b2BodyPtr->GetAngle() - fk::TAU / 8;
							spriteBatch[spriteIDs[2]].setColor(255, 255, 255, 255);
							m_rightSwipe = 15;
							++p_drainCount.right;
						} else {
							spriteBatch[spriteIDs[4]].setPosition(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[4]].setRotationAxis(
								p_targetPtr->b2BodyPtr->GetPosition().x, p_targetPtr->b2BodyPtr->GetPosition().y
							);
							spriteBatch[spriteIDs[4]].canvas.rotationAngle = b2BodyPtr->GetAngle() - fk::TAU / 8;
							spriteBatch[spriteIDs[4]].setColor(255, 255, 255, 255);
							p_targetPtr->b2BodyPtr->ApplyLinearImpulse(
								b2Vec2(p_faceDirection.x * -7, p_faceDirection.y * -7),
								p_targetPtr->b2BodyPtr->GetPosition(),
								true
							);
							p_targetPtr->health -= 1;
							p_drainCount.right = 0;
						}
						p_targetPtr->hit = true;
					}
				}
			} else { p_targetPtr = nullptr; }
		}
		if (move) {
			float dodgeMult = 1;
			if (p_dodging > m_dodgeIFrames - 3) { dodgeMult = 16; }
			b2BodyPtr->ApplyLinearImpulse(
				b2Vec2(p_moveDirection.x * dodgeMult, p_moveDirection.y * dodgeMult),
				b2BodyPtr->GetWorldCenter(),
				true
			);
		}
		if (p_dodging) { if (--p_dodging == 0) { m_dodgeCharge = 0; } }
		if (m_dodgeTimer) { --m_dodgeTimer; }
	}
}
void Player::updateSprite() {
	spriteBatch[spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[1]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[2]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	if (m_charging) {
		spriteBatch[spriteIDs[0]].setColor(255, 100, 100, 255);
	} else if (m_dodgeCharge > 20) {
		spriteBatch[spriteIDs[0]].setColor(100, 100, 255, 255);
	} else {
		spriteBatch[spriteIDs[0]].setColor(255, 255, 255, 255);
	}
	if (health > 0) {
		int alpha = spriteBatch[spriteIDs[1]].canvas.color.a - 10;
		if (alpha < 0) { alpha = 0; }
		spriteBatch[spriteIDs[1]].setColor(255, 255, 255, alpha);
		alpha = spriteBatch[spriteIDs[3]].canvas.color.a - 10;
		if (alpha < 0) { alpha = 0; }
		spriteBatch[spriteIDs[3]].setColor(255, 255, 255, alpha);

		alpha = spriteBatch[spriteIDs[2]].canvas.color.a - 10;
		if (alpha < 0) { alpha = 0; }
		spriteBatch[spriteIDs[2]].setColor(255, 255, 255, alpha);
		alpha = spriteBatch[spriteIDs[4]].canvas.color.a - 10;
		if (alpha < 0) { alpha = 0; }
		spriteBatch[spriteIDs[4]].setColor(255, 255, 255, alpha);
	} else {
		spriteBatch[spriteIDs[0]].setColor(255, 255, 255, 100);
		spriteBatch[spriteIDs[1]].setColor(255, 255, 255, 0);
		spriteBatch[spriteIDs[2]].setColor(255, 255, 255, 0);
		spriteBatch[spriteIDs[3]].setColor(255, 255, 255, 0);
		spriteBatch[spriteIDs[4]].setColor(255, 255, 255, 0);
	}
}

float32 Player::ReportFixture(
	b2Fixture* fixturePtr,
	const b2Vec2& point,
	const b2Vec2& normal,
	float32 fraction
) {
	if (static_cast<Object*>(fixturePtr->GetBody()->GetUserData())->category == "actor") {
		p_targetPtr = static_cast<Actor*>(fixturePtr->GetBody()->GetUserData());
	}
	return fraction;
}
