#include "Player.h"
#include "base/Utility.h"

Player::Player(
	Map& map,
	fk::UserInput* uiPtr,
	ActorDef& ad
) : Actor(map, ad, *(new M_Control(*this)), VOID), m_uiPtr(uiPtr) {
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
	health = 5000;
	b2FixtureDef fixtureDef;
	b2CircleShape circle;
	circle.m_radius = ad.size * 1.5;
	fixtureDef.shape = &circle;
	fixtureDef.isSensor = true;
	fixtureDef.userData = (void*)'r';
	b2BodyPtr->CreateFixture(&fixtureDef);
	movement.speed = 1;
}
Player::~Player() {

}
void Player::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Blood levels.
	int fireFloor = floor(blood.fire);
	int waterFloor = floor(blood.water);
	int earthFloor = floor(blood.earth);
	int airFloor = floor(blood.air);
	if (m_oldFloor.fire < fireFloor) { drainTimer.fire = 60 * 20; }
	if (m_oldFloor.water < fireFloor) { drainTimer.water = 60 * 20; }
	if (m_oldFloor.earth < fireFloor) { drainTimer.earth = 60 * 20; }
	if (m_oldFloor.air < fireFloor) { drainTimer.air = 60 * 20; }
	m_oldFloor.fire = fireFloor;
	m_oldFloor.water = fireFloor;
	m_oldFloor.earth = fireFloor;
	m_oldFloor.air = fireFloor;
	if (drainTimer.fire) { --drainTimer.fire; }
	else if (blood.fire >= 1) {
		if (blood.fire >= 2) { drainTimer.fire = 60 * 20; }
		blood.fire -= 1;
	}
	if (drainTimer.water) { --drainTimer.water; }
	else if (blood.water >= 1) {
		if (blood.water >= 2) { drainTimer.water = 60 * 20; }
		blood.water -= 1;
	}
	if (drainTimer.earth) { --drainTimer.earth; }
	else if (blood.earth >= 1) {
		if (blood.earth >= 2) { drainTimer.earth = 60 * 20; }
		blood.earth -= 1;
	}
	if (drainTimer.air) { --drainTimer.air; }
	else if (blood.air >= 1) {
		if (blood.air >= 2) { drainTimer.air = 60 * 20; }
		blood.air -= 1;
	}
	if (hit) {
		if ((blood.fire -= 0.25) <= 0) { blood.fire = 0; }
		if ((blood.water -= 0.25) <= 0) { blood.water = 0; }
		if ((blood.earth -= 0.25) <= 0) { blood.earth = 0; }
		if ((blood.air -= 0.25) <= 0) { blood.air = 0; }
	}
	// Timers
	if (m_leftSwipeTimer == -30) { m_leftSwipes = 0; }
	if (m_rightSwipeTimer == -30) { m_rightSwipes = 0; }
	if (m_leftSwipeTimer > -30) { --m_leftSwipeTimer; }
	if (m_rightSwipeTimer > -30) { --m_rightSwipeTimer; }
	if (m_dodgeTimer > 0) { --m_dodgeTimer; }
	// Mouse.
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).position);
	// States.
	if (health < 1) { setState(new Dead(*this)); }
	states.currentPtr->think(actorPtrs, camPtr);
	hit = false;
}
void Player::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (!collisionFixturePtr->IsSensor()) {
		if (movement.dodging) {
			Object* op = static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData());
			if (op->category == "actor") {
				///static_cast<Actor*>(op)->pause(60);
				static_cast<Actor*>(op)->hit = true;
			} else if (op->category == "static" && !myFixturePtr->GetUserData()) {
				///movement.dodging = 0;
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
void Player::updateSprite() {
	int alpha;
	for (int i = 1; i < spriteIDs.size(); ++i) {
		alpha = spriteBatch[spriteIDs[i]].canvas.color.a - 10;
		if (alpha < 0) { alpha = 0; }
		spriteBatch[spriteIDs[i]].canvas.color.a = alpha;
	}
	Actor::updateSprite();
}
float32 Player::ReportFixture(
	b2Fixture* fixturePtr,
	const b2Vec2& point,
	const b2Vec2& normal,
	float32 fraction
) {
	if (static_cast<Object*>(fixturePtr->GetBody()->GetUserData())->category == "actor") {
		targetInfo.ptr = static_cast<Actor*>(fixturePtr->GetBody()->GetUserData());
	}
	return fraction;
}


void Player::M_Control::enter() {
	actorPtr->spriteBatch[actorPtr->spriteIDs[0]].setColor(255, 255, 255, 255);
}
void Player::M_Control::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	// Face direction
	if (player.m_mousePos != player.getPosition()) {
		player.los.faceDirection = glm::normalize(player.getPosition() - player.m_mousePos);
	}
	// Move direction
	player.movement.vector.x = 0;
	player.movement.vector.y = 0;
	if (player.m_uiPtr->getKeyInfo(fk::Key::W).downFrames > 0) { player.movement.vector.y += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::S).downFrames > 0) { player.movement.vector.y -= 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::D).downFrames > 0) { player.movement.vector.x += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::A).downFrames > 0) { player.movement.vector.x -= 1; }
	if (player.movement.vector.x || player.movement.vector.y) {
		float boost{ 0 };
		if (player.m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames > 0) { boost = { 0.75 }; }
		player.movement.vector = (player.movement.speed + boost) * glm::normalize(player.movement.vector);
	}
	// State change
	if (player.m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames == 1 && !player.m_dodgeTimer) {
		player.m_readyToDash = false;
		player.setState(new M_Dodge(*actorPtr));
	} else if (player.m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames) {
		player.m_readyToDash = false;
		player.setState(new M_Cast(*actorPtr));
	} else if (
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1 ||
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1
	) {
		if (glm::length(player.m_mousePos - player.getPosition()) <= player.getRadius()) {
			player.m_readyToDash = true;
		} else if (
			player.m_leftSwipeTimer <= 0 &&
			player.m_leftSwipes < player.m_leftMaxSwipes &&
			player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1 ||
			player.m_rightSwipeTimer <= 0 &&
			player.m_rightSwipes < player.m_rightMaxSwipes &&
			player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1
		) {
			player.m_readyToDash = false;
			player.setState(new M_Attack(*actorPtr));
		}
	} else if (
		player.m_readyToDash && !player.m_dodgeTimer &&
		!player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames &&
		!player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
	) {
		player.m_readyToDash = false;
		player.setState(new M_StunDash(*actorPtr));
	}
}


void Player::M_StunDash::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	player.movement.dodging = true;
	player.movement.vector = player.m_mousePos - player.getPosition();
	if (player.movement.vector.x || player.movement.vector.y) {
		player.movement.vector = glm::normalize(player.movement.vector);
	}
	b2Filter filter = player.b2BodyPtr->GetFixtureList()->GetNext()->GetFilterData();
	filter.maskBits = 1;
	player.b2BodyPtr->GetFixtureList()->GetNext()->SetFilterData(filter);
	player.m_dodgeTimer = 40;
	player.spriteBatch[player.spriteIDs[0]].setColor(100, 100, 255, 255);
}
void Player::M_StunDash::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	if (!iFrames--) {
		actorPtr->movement.dodging = false;
		b2Filter filter = actorPtr->b2BodyPtr->GetFixtureList()->GetNext()->GetFilterData();
		filter.maskBits = 0b1111111111111111;
		actorPtr->b2BodyPtr->GetFixtureList()->GetNext()->SetFilterData(filter);
		actorPtr->setState(new M_Control(*actorPtr));
	}
}
void Player::M_StunDash::updateBody() {
	if (iFrames > 10) {
		actorPtr->b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(actorPtr->movement.vector.x * 16, actorPtr->movement.vector.y * 16),
			actorPtr->b2BodyPtr->GetWorldCenter(),
			true
		);
	}
}


void Player::M_Attack::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	player.movement.vector.x = 0;
	player.movement.vector.y = 0;
	mouseButtons = {
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames,
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
	};
	player.spriteBatch[actorPtr->spriteIDs[0]].setColor(255, 200, 200, 255);
	// Get target
	player.targetInfo.ptr = nullptr;
	glm::vec2 rayCast = glm::normalize(player.m_mousePos - player.getPosition());
	rayCast *= 1;
	glm::vec2 leftRayCast = fk::rotatePoint(rayCast, fk::TAU / 20);
	glm::vec2 rightRayCast = fk::rotatePoint(rayCast, -fk::TAU / 20);
	// Cast 3 rays
	player.b2BodyPtr->GetWorld()->RayCast(
		actorPtr,
		b2Vec2(player.getPosition().x, player.getPosition().y),
		b2Vec2(player.getPosition().x + rayCast.x, player.getPosition().y + rayCast.y)
	);
	if (!player.targetInfo.ptr) {
		player.b2BodyPtr->GetWorld()->RayCast(
			actorPtr,
			b2Vec2(player.getPosition().x, player.getPosition().y),
			b2Vec2(player.getPosition().x + leftRayCast.x, player.getPosition().y + leftRayCast.y)
		);
		player.b2BodyPtr->GetWorld()->RayCast(
			actorPtr,
			b2Vec2(player.getPosition().x, player.getPosition().y),
			b2Vec2(player.getPosition().x + rightRayCast.x, player.getPosition().y + rightRayCast.y)
		);
	}
	if (!player.targetInfo.ptr) { player.returnToPrevState(); return; }
	else {
		for (int i = 0; i < mouseButtons.size(); ++i) {
			if (
				mouseButtons[i] == 1 &&
				(i ? player.m_rightSwipeTimer <= 0 : player.m_leftSwipeTimer <= 0) && (
					i ? player.m_rightSwipes < player.m_rightMaxSwipes :
					player.m_leftSwipes < player.m_leftMaxSwipes
				)
			) {
				// Attack
				player.targetInfo.ptr->health -= 1;
				--drainCount;
				if ((player.blood.fire += 0.03) >= 7) { player.blood.fire = 6.99; }
				if ((player.blood.water += 0.03) >= 7) { player.blood.water = 6.99; }
				if ((player.blood.earth += 0.03) >= 7) { player.blood.earth = 6.99; }
				if ((player.blood.air += 0.03) >= 7) { player.blood.air = 6.99; }
				(i ? player.m_rightSwipeTimer : player.m_leftSwipeTimer) =
					(i ? player.m_rightSwipeRest : player.m_leftSwipeRest);
				i ? ++player.m_rightSwipes : ++player.m_leftSwipes;
			}
		}
	}
}
void Player::M_Attack::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	// If no target cancel attack
	mouseButtons = {
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames,
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
	};
	// State switch
	if (!drainCount || !mouseButtons[0] && !mouseButtons[1]) {
		player.m_readyToDash = false;
		player.setState(new M_Control(*actorPtr));
	} else if (player.m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames == 1 && !player.m_dodgeTimer) {
		player.m_readyToDash = false;
		player.setState(new M_Dodge(*actorPtr));
	} else if (
		glm::length(player.m_mousePos - player.getPosition()) <= player.getRadius() &&
		(mouseButtons[0] == 1 || mouseButtons[1] == 1)
	) {
		player.m_readyToDash = true;
		dodgeButton = mouseButtons[0] == 1 ? 0 : 1;
	} else if (player.m_readyToDash && !player.m_dodgeTimer && !mouseButtons[dodgeButton]) {
		player.m_readyToDash = false;
		player.setState(new M_StunDash(*actorPtr));
	}
	// Face direction
	if (player.getPosition() != player.targetInfo.ptr->getPosition()) {
		player.los.faceDirection =
			glm::normalize(player.getPosition() - player.targetInfo.ptr->getPosition());
	}
	///p_targetPtr->pause(30);
	for (int i = 0; i < mouseButtons.size(); ++i) {
		if (mouseButtons[i]) {
			if (
				mouseButtons[i] == 1 &&
				(i ? player.m_rightSwipeTimer <= 0 : player.m_leftSwipeTimer <= 0) && (
					i ? player.m_rightSwipes < player.m_rightMaxSwipes :
					player.m_leftSwipes < player.m_leftMaxSwipes
				)
			) {
				// Attack
				player.targetInfo.ptr->health -= 1;
				--drainCount;
				if ((player.blood.fire += 0.03) >= 7) { player.blood.fire = 6.99; }
				if ((player.blood.water += 0.03) >= 7) { player.blood.water = 6.99; }
				if ((player.blood.earth += 0.03) >= 7) { player.blood.earth = 6.99; }
				if ((player.blood.air += 0.03) >= 7) { player.blood.air = 6.99; }
				(i ? player.m_rightSwipeTimer : player.m_leftSwipeTimer) =
					(i ? player.m_rightSwipeRest : player.m_leftSwipeRest);
				i ? ++player.m_rightSwipes : ++player.m_leftSwipes;
			}
			if (drainCount && mouseButtons[i] % 15 == 0) {
				// Drain
				--drainCount;
				if ((player.blood.fire += 0.03) >= 7) { player.blood.fire = 6.99; }
				if ((player.blood.water += 0.03) >= 7) { player.blood.water = 6.99; }
				if ((player.blood.earth += 0.03) >= 7) { player.blood.earth = 6.99; }
				if ((player.blood.air += 0.03) >= 7) { player.blood.air = 6.99; }
			}
			player.targetInfo.ptr->hit = true;
		}
	}
}
void Player::M_Attack::updateBody() {
	State::updateBody();
	Player& player = *static_cast<Player*>(actorPtr);
	// Lunging
	glm::vec2 targetVector = player.getPosition() - player.targetInfo.ptr->getPosition();
	if (glm::length(targetVector) >= 0.6 && (mouseButtons[0] || mouseButtons[1])) {
		player.b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(-targetVector.x * 2, -targetVector.y * 2),
			player.b2BodyPtr->GetWorldCenter(),
			true
		);
	}
	// Knockback
	for (int i = 0; i < mouseButtons.size(); ++i) {
		if (
			mouseButtons[i] == 1 &&
			(i ? player.m_rightSwipeTimer : player.m_leftSwipeTimer) ==
			(i ? player.m_rightSwipeRest : player.m_leftSwipeRest) && (
				i ? player.m_rightSwipes <= player.m_rightMaxSwipes :
				player.m_leftSwipes <= player.m_leftMaxSwipes
			) || !drainCount
		) {
			// Attack/drain knock back
			player.targetInfo.ptr->b2BodyPtr->ApplyLinearImpulse(
				b2Vec2(player.los.faceDirection.x * -10, player.los.faceDirection.y * -10),
				player.targetInfo.ptr->b2BodyPtr->GetPosition(),
				true
			);
		}
	}
}
void Player::M_Attack::updateSprite() {
	State::updateSprite();
	Player& player = *static_cast<Player*>(actorPtr);
	for (int i = 0; i < mouseButtons.size(); ++i) {
		if (mouseButtons[i]) {
			if (
				mouseButtons[i] == 1 &&
				(i ? player.m_rightSwipeTimer : player.m_leftSwipeTimer) ==
				(i ? player.m_rightSwipeRest : player.m_leftSwipeRest) && (
					i ? player.m_rightSwipes <= player.m_rightMaxSwipes :
					player.m_leftSwipes <= player.m_leftMaxSwipes
				)
			) {
				// Attack
				player.spriteBatch[player.spriteIDs[3 + i]].setPosition(
					player.targetInfo.ptr->b2BodyPtr->GetPosition().x,
					player.targetInfo.ptr->b2BodyPtr->GetPosition().y
				);
				player.spriteBatch[player.spriteIDs[3 + i]].setRotationAxis(
					player.targetInfo.ptr->b2BodyPtr->GetPosition().x,
					player.targetInfo.ptr->b2BodyPtr->GetPosition().y
				);
				player.spriteBatch[player.spriteIDs[3 + i]].canvas.rotationAngle =
					player.b2BodyPtr->GetAngle() + (i ? fk::TAU / 8 : -fk::TAU / 8);
				player.spriteBatch[player.spriteIDs[3 + i]].setColor(255, 255, 255, 255);
			} else if (drainCount && mouseButtons[i] % 15 == 0) {
				// Drain
				player.spriteBatch[player.spriteIDs[2 - i]].setPosition(
					player.targetInfo.ptr->b2BodyPtr->GetPosition().x,
					player.targetInfo.ptr->b2BodyPtr->GetPosition().y
				);
				player.spriteBatch[player.spriteIDs[2 - i]].setRotationAxis(
					player.targetInfo.ptr->b2BodyPtr->GetPosition().x,
					player.targetInfo.ptr->b2BodyPtr->GetPosition().y
				);
				player.spriteBatch[player.spriteIDs[2 - i]].canvas.rotationAngle =
					player.b2BodyPtr->GetAngle();
				player.spriteBatch[player.spriteIDs[2 - i]].setColor(255, 255, 255, 255);
			}
		}
	}
}


void Player::M_Dodge::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	player.movement.vector.x = 0;
	player.movement.vector.y = 0;
	player.movement.dodging = true;
	player.m_dodgeTimer = 25;
	b2Filter filter = player.b2BodyPtr->GetFixtureList()->GetNext()->GetFilterData();
	filter.maskBits = 1;
	player.b2BodyPtr->GetFixtureList()->GetNext()->SetFilterData(filter);
	player.spriteBatch[player.spriteIDs[0]].setColor(100, 100, 255, 255);
}
void Player::M_Dodge::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	if (!iFrames--) {
		actorPtr->movement.dodging = false;
		b2Filter filter = actorPtr->b2BodyPtr->GetFixtureList()->GetNext()->GetFilterData();
		filter.maskBits = 0b1111111111111111;
		actorPtr->b2BodyPtr->GetFixtureList()->GetNext()->SetFilterData(filter);
		actorPtr->setState(new M_Control(*actorPtr));
	}
}
void Player::M_Dodge::updateBody() {
	if (iFrames > 12) {
		actorPtr->b2BodyPtr->ApplyLinearImpulse(
			b2Vec2(actorPtr->los.faceDirection.x * 8, actorPtr->los.faceDirection.y * 8),
			actorPtr->b2BodyPtr->GetWorldCenter(),
			true
		);
	}
}


void Player::M_Cast::enter() {
	actorPtr->movement.vector.x = 0;
	actorPtr->movement.vector.y = 0;
	actorPtr->spriteBatch[actorPtr->spriteIDs[0]].setColor(255, 255, 255, 255);
}
void Player::M_Cast::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	// State change
	if (player.m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames && !player.m_dodgeTimer) {
		player.m_readyToDash = false;
		player.setState(new M_Dodge(*actorPtr));
	} else if (!player.m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames) {
		player.m_readyToDash = false;
		player.setState(new M_Control(*actorPtr));
	}
}
