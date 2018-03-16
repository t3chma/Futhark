#include "Player.h"
#include "base/Utility.h"

Player::Player(
	fk::SpriteBatch* sbPtr,
	b2World* wPtr,
	fk::UserInput* uiPtr,
	fk::ActorDef& ad
) : Actor(sbPtr, wPtr, ad), m_uiPtr(uiPtr) {
	(*p_sbPtr)[p_spriteIDs[1]].setDimensions(0.2, 0.2);
	(*p_sbPtr)[p_spriteIDs[2]].setDimensions(2, 2);
	(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, 0);
	(*p_sbPtr)[p_spriteIDs[3]].setDimensions(2, 2);
	(*p_sbPtr)[p_spriteIDs[3]].setColor(255, 255, 255, 0);
	(*p_sbPtr)[p_spriteIDs[3]].setTextureDimensions(-1, 1);
	(*p_sbPtr)[p_spriteIDs[3]].setTexturePosition(1, 0);
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
	if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1) { m_leftSwipe = true; }
	else { m_leftSwipe = false; }
	if (m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1) { m_rightSwipe = true; }
	else { m_rightSwipe = false; }
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).position);
	glm::vec2 lookVec = glm::normalize(m_mousePos - glm::vec2(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y));
	float lookAngle = fk::makeAngle(lookVec) + fk::TAU/4;
	for (auto&& spriteID : p_spriteIDs) {
		(*p_sbPtr)[spriteID].canvas.rotationAngle = lookAngle;
	}
	if (p_moveDirection.x || p_moveDirection.y) {
		p_moveDirection = glm::vec2(p_speed) * glm::normalize(p_moveDirection);
	}
}
void Player::updateBody() {
	p_bodyPtr->ApplyLinearImpulse(
		b2Vec2(p_moveDirection.x, p_moveDirection.y),
		p_bodyPtr->GetPosition(),
		true
	);
}
void Player::updateSprite() {
	(*p_sbPtr)[p_spriteIDs[0]].setPosition(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].setRotationAxis(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[1]].setPosition(m_mousePos.x, m_mousePos.y);
	(*p_sbPtr)[p_spriteIDs[1]].setRotationAxis(m_mousePos.x, m_mousePos.y);
	if (m_leftSwipe) {
		(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, 255);
		(*p_sbPtr)[p_spriteIDs[2]].setPosition(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
		(*p_sbPtr)[p_spriteIDs[2]].setRotationAxis(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
	} else {
		int alpha = (*p_sbPtr)[p_spriteIDs[2]].canvas.color.a - 40;
		if (alpha < 0) { alpha = 0; }
		(*p_sbPtr)[p_spriteIDs[2]].setColor(255, 255, 255, alpha);
	}
	if (m_rightSwipe) {
		(*p_sbPtr)[p_spriteIDs[3]].setColor(255, 255, 255, 255);
		(*p_sbPtr)[p_spriteIDs[3]].setPosition(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
		(*p_sbPtr)[p_spriteIDs[3]].setRotationAxis(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
	} else {
		int alpha = (*p_sbPtr)[p_spriteIDs[3]].canvas.color.a - 40;
		if (alpha < 0) { alpha = 0; }
		(*p_sbPtr)[p_spriteIDs[3]].setColor(255, 255, 255, alpha);
	}
}