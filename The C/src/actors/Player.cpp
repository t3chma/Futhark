#include "Player.h"
#include "base/Utility.h"

Player::Player(
	fk::World& world,
	fk::UserInput* uiPtr,
	PlayerDef& pd
) : Actor(world, pd.ad, *(new M_Control(*this)), nullptr), m_uiPtr(uiPtr) {
	// Misc
	health = 5000;
	type = "player";
	// Graphics
	for (auto&& sprite : sprites) { sprite.setDimensions(pd.ad.size / 2.0f, pd.ad.size / 2.0f); }
	spritePtrs.body = &sprites.front();
	spritePtrs.body->getCanvasRef().position.z = 1;
	spritePtrs.body->setColor(255, 255, 255, 255);
	spritePtrs.body->setDimensions(pd.ad.size, pd.ad.size);

	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	// AOE attack
	addCircleLimb(1, 0, 0, &fixtureDef).category = "S";
}
Player::~Player() {

}
void Player::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Mouse
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).windowPosition);
	// States
	if (health < 1) { setState(new Dead(*this)); }
	states.currentPtr->think(actorPtrs, camPtr);
	for (int i = _TRAIL_ - 1; i > 0; --i) { m_oldPos[i] = m_oldPos[i - 1]; }
	m_oldPos[0] = getPosition();
	for (int i = _TRAIL_ - 1; i > 0; --i) { m_oldAng[i] = m_oldAng[i - 1]; }
	auto velocity = b2Ptr->GetLinearVelocity();
	auto vel = glm::vec2(velocity.x, velocity.y);
	if (vel.x || vel.y) {
		m_oldAng[0] = fk::makeAngle(glm::normalize(vel)) - fk::TAU / 4;
		m_oldVel = vel;
	} else {
		m_oldAng[0] = m_oldAng[1];
	}
}
void Player::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
}
void Player::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
}
void Player::updateSprites() {
	int i = 0;
	for (auto&& sprite : sprites) {
		if (&sprite == &sprites.front()) {
			sprite.setRotationAngle(b2Ptr->GetAngle());
			sprite.setPosition(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
			sprite.setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
		} else {
			sprite.setRotationAngle(m_oldAng[i]);
			sprite.setPosition(m_oldPos[i].x, m_oldPos[i].y);
			sprite.setRotationAxis(m_oldPos[i].x, m_oldPos[i].y);
			sprite.setDimensions(glm::vec2(p_radius) + glm::vec2(0.02 * i));
			auto velocity = b2Ptr->GetLinearVelocity();
			sprite.setColor(255, 255, 255, (100 - i) / 8);
			++i;
		}
	}
	states.currentPtr->updateSprite();
}
float32 Player::ReportFixture(
	b2Fixture* fixturePtr,
	const b2Vec2& point,
	const b2Vec2& normal,
	float32 fraction
) {
	p_raycast.target = static_cast<Object*>(fixturePtr->GetBody()->GetUserData());
	p_raycast.fraction = fraction;
	return fraction;
}


void Player::M_Control::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	actorPtr->sprites.front().setColor(255, 255, 255, 255);
}
void Player::M_Control::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	b2Vec2 test;
	// Face direction
	if (player.m_mousePos != player.getPosition()) {
		player.los.faceDirection = glm::normalize(player.getPosition() - player.m_mousePos);
	}
	float chargingMoveDampen = 0;
	int charge[2] = {
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames,
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
	};
	// Move direction
	player.movement.direction.x = 0;
	player.movement.direction.y = 0;
	if (player.m_uiPtr->getKeyInfo(fk::Key::W).downFrames > 0) { player.movement.direction.y += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::S).downFrames > 0) { player.movement.direction.y -= 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::D).downFrames > 0) { player.movement.direction.x += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::A).downFrames > 0) { player.movement.direction.x -= 1; }
	int shiftFrames = player.m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames;
	if (player.movement.direction.x || player.movement.direction.y) {
		float boost{ 0 };
		if (shiftFrames > 0) { boost = { 0.75 }; }
		player.movement.direction = (
			player.movement.speed + boost - chargingMoveDampen
		) * glm::normalize(player.movement.direction);
	}
}
void Player::M_Control::updateBody() {
	State::updateBody();
}
void Player::M_Control::updateSprite() {
	Player& player = *static_cast<Player*>(actorPtr);
}