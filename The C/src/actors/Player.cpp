#include "Player.h"


Player::Player(Player::Def& pd) : Boat(pd, *(new M_Control(*this))) {
	type = "player";
}

Player::~Player() {

}

void Player::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Mouse
	m_mousePos = camPtr->getWorldCoordinates(p_uiPtr->getMouseInfo(0).windowPosition);
	Boat::think(actorPtrs, camPtr);
}

void Player::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	auto objectPtr = static_cast<Object*>(
		collisionFixturePtr->GetBody()->GetUserData()
	);
	if (objectPtr->category == "mouse") { m_moused = true; }
}

void Player::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	auto objectPtr = static_cast<Object*>(
		collisionFixturePtr->GetBody()->GetUserData()
	);
	if (objectPtr->category == "mouse") { m_moused = true; }
}

void Player::M_Control::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
}

void Player::M_Control::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	b2Vec2 test;
	// Move direction
	if (player.p_uiPtr->getKeyInfo(fk::Key::D).downFrames) {
		player.los.faceDirection = fk::rotatePoint(player.los.faceDirection, -fk::TAU / 500);
	}
	if (player.p_uiPtr->getKeyInfo(fk::Key::A).downFrames) {
		player.los.faceDirection = fk::rotatePoint(player.los.faceDirection, fk::TAU / 500);
	}
	// Throtle
	static int gear = 0;
	if (player.p_uiPtr->getKeyInfo(fk::Key::W).downFrames == 1 && gear < 4) { gear += 1; }
	if (player.p_uiPtr->getKeyInfo(fk::Key::S).downFrames == 1 && gear > -1) { gear -= 1; }
	int shiftFrames = player.p_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames;
	auto direction = glm::normalize(player.los.faceDirection);
	if (gear && (direction.x || direction.y)) {
		float boost{ shiftFrames > 0 ? 0.75f : 0 };
		player.movement.direction = gear * (player.movement.speed + boost) * direction;
	} else {
		player.movement.direction = glm::vec2(0);
	}
	fk::Random rangen;
	static int i = 1;
	if (rangen.getInt(0, 3)) {
		player.movement.direction.x += rangen.getFloat(0, 1) * i;
		player.movement.direction.y += rangen.getFloat(0, 1) * i;
		i = -i;
	}
}