#include "Player.h"
#include "base/Utility.h"

Player::Player(
	fk::SpriteBatch* sbPtr,
	fk::World& world,
	fk::UserInput* uiPtr,
	fk::ActorDef& ad
) : Actor(sbPtr, world, ad), m_uiPtr(uiPtr) {
	(*p_sbPtr)[p_spriteIDs[3]].setDimensions(0.2, 0.2);
	(*p_sbPtr)[p_spriteIDs[1]].setDimensions(ad.size*3, ad.size*4);
	(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 255, 0);
	(*p_sbPtr)[p_spriteIDs[2]].setDimensions(ad.size*3, ad.size*4);
	(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, 0);
	(*p_sbPtr)[p_spriteIDs[2]].setTextureDimensions(-1, 1);
	(*p_sbPtr)[p_spriteIDs[2]].setTexturePosition(1, 0);
	b2FixtureDef fixtureDef;
	b2CircleShape circle;
	fixtureDef.shape = &circle;
	circle.m_radius = ad.size * 2;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.isSensor = true;
	fixtureDef.userData = (void*)'s';
	b2BodyPtr->CreateFixture(&fixtureDef);
	b2PolygonShape box;
	fixtureDef.shape = &box;
	b2Vec2 points1[]{
		b2Vec2(-ad.size*0.5, 0), b2Vec2(-ad.size*0.5, -ad.size*2),
		b2Vec2(ad.size*1.5, -ad.size*2), b2Vec2(ad.size*1.5, 0)
	};
	box.Set(points1, 4);
	fixtureDef.userData = (void*)'l';
	b2BodyPtr->CreateFixture(&fixtureDef);
	b2Vec2 points2[]{
		b2Vec2(-ad.size*1.5, 0), b2Vec2(-ad.size*1.5, -ad.size*2),
		b2Vec2(ad.size*0.5, -ad.size*2), b2Vec2(ad.size*0.5, 0)
	};
	box.Set(points2, 4);
	fixtureDef.userData = (void*)'r';
	b2BodyPtr->CreateFixture(&fixtureDef);
	category = "Player";
	p_health = 50;
}
Player::~Player() {

}
void Player::think(std::vector<fk::Actor*>& actorPtrs, fk::Camera* camPtr) {
	p_speed = 0.5;
	p_moveDirection.x = 0;
	p_moveDirection.y = 0;
	if (m_uiPtr->getKeyInfo(fk::Key::W).downFrames > 1) { p_moveDirection.y += 1; }
	if (m_uiPtr->getKeyInfo(fk::Key::S).downFrames > 1) { p_moveDirection.y -= 1; }
	if (m_uiPtr->getKeyInfo(fk::Key::D).downFrames > 1) { p_moveDirection.x += 1; }
	if (m_uiPtr->getKeyInfo(fk::Key::A).downFrames > 1) { p_moveDirection.x -= 1; }
	if (m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames > 1) { p_speed = 1; }
	if (
		m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1
		&& m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames == 0
	) { m_leftSwipe = true; }
	else { m_leftSwipe = false; }
	if (
		m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1
		&& m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames == 0
	) { m_rightSwipe = true; }
	else { m_rightSwipe = false; }
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).position);
	glm::vec2 lookVec = glm::normalize(m_mousePos - glm::vec2(
		b2BodyPtr->GetPosition().x,
		b2BodyPtr->GetPosition().y
	));
	if (lookVec.x || lookVec.y) { p_faceDirection = glm::normalize(lookVec); }
	p_faceAngle = fk::makeAngle(p_faceDirection) + fk::TAU/4;
	if (p_moveDirection.x || p_moveDirection.y) { p_moveDirection = p_speed * glm::normalize(p_moveDirection); }
	if (p_hit) { p_health -= p_hit; p_hit = 0; }
}
void Player::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else {
		if (myFixturePtr->GetUserData() != nullptr) {
			switch ((char)myFixturePtr->GetUserData()) {
			  case 's':
				m_swipeRange = true;
			  break;
			  case 'l':
				m_leftHitPtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  case 'r':
				m_rightHitPtrs.push_back(static_cast<fk::Body*>(collisionFixturePtr->GetBody()->GetUserData()));
			  break;
			  default:
			  break;
			}
		}
	}
}
void Player::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (collisionFixturePtr->IsSensor()) {
	} else {
		if (myFixturePtr->GetUserData() != nullptr) {
			switch ((char)myFixturePtr->GetUserData()) {
			  case 's':
				m_swipeRange = false;
			  break;
			  case 'l':
				m_leftHitPtrs.clear();
			  break;
			  case 'r':
				m_rightHitPtrs.clear();
			  break;
			  default:
			  break;
			}
		}
	}
}
void Player::updateBody() {
	if (p_health > 0) {
		b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(p_moveDirection.x, p_moveDirection.y),
			b2BodyPtr->GetWorldCenter(),
			true
		);
		b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), p_faceAngle);
		if (m_swipeRange) {
			if (m_leftSwipe) {
				for (auto&& hitBodyPtr : m_leftHitPtrs) {
					hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
						b2Vec2(p_faceDirection.x * 2.5, p_faceDirection.y * 2.5),
						hitBodyPtr->b2BodyPtr->GetPosition(),
						true
					);
					++hitBodyPtr->p_hit;
				}
			}
			if (m_rightSwipe) {
				for (auto&& hitBodyPtr : m_rightHitPtrs) {
					hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
						b2Vec2(p_faceDirection.x * 2.5, p_faceDirection.y * 2.5),
						hitBodyPtr->b2BodyPtr->GetPosition(),
						true
					);
					++hitBodyPtr->p_hit;
				}
			}
		}
	}
}
void Player::updateSprite() {
	for (auto&& spriteID : p_spriteIDs) { (*p_sbPtr)[spriteID].canvas.rotationAngle = b2BodyPtr->GetAngle(); }
	(*p_sbPtr)[p_spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[3]].setPosition(m_mousePos.x, m_mousePos.y);
	(*p_sbPtr)[p_spriteIDs[3]].setRotationAxis(m_mousePos.x, m_mousePos.y);
	if (p_health > 0) {
		if (m_rightSwipe) {
			(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 255, 255);
			(*p_sbPtr)[p_spriteIDs[1]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			(*p_sbPtr)[p_spriteIDs[1]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = (*p_sbPtr)[p_spriteIDs[1]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 255, alpha);
		}
		if (m_leftSwipe) {
			(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, 255);
			(*p_sbPtr)[p_spriteIDs[2]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
			(*p_sbPtr)[p_spriteIDs[2]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
		} else {
			int alpha = (*p_sbPtr)[p_spriteIDs[2]].canvas.color.a - 100;
			if (alpha < 0) { alpha = 0; }
			(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, alpha);
		}
	} else {
		(*p_sbPtr)[p_spriteIDs[0]].setColor(255, 255, 255, 100);
		(*p_sbPtr)[p_spriteIDs[1]].setColor(255, 255, 255, 0);
		(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, 0);
	}
}