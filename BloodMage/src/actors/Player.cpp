#include "Player.h"
#include "base/Utility.h"
#include "../SpellCatalog.h"

Player::Player(
	Map& map,
	fk::UserInput* uiPtr,
	PlayerDef& pd
) : Actor(map, pd.ad, *(new M_Control(*this)), nullptr), m_uiPtr(uiPtr) {
	health = 5000;
	movement.speed = 1;
	type = "player";
	enchants.melee.resize(4);
	sprites.add("leftSword", pd.sword);
	sprites.get("leftSword")->setColor(255, 255, 255, 0);
	sprites.get("leftSword")->setDimensions(pd.ad.size, pd.ad.size);
	sprites.add("rightSword", pd.sword);
	sprites.get("rightSword")->setColor(255, 255, 255, 0);
	sprites.get("rightSword")->setDimensions(pd.ad.size, pd.ad.size);
	sprites.get("rightSword")->setTextureDimensions(-1, 1);
	sprites.get("rightSword")->setTexturePosition(1, 0);
	sprites.add("leftSwipe", pd.swipe);
	sprites.get("leftSwipe")->setColor(255, 255, 255, 0);
	sprites.get("leftSwipe")->setDimensions(pd.ad.size * 2, pd.ad.size * 3);
	sprites.add("rightSwipe", pd.swipe);
	sprites.get("rightSwipe")->setColor(255, 255, 255, 0);
	sprites.get("rightSwipe")->setDimensions(pd.ad.size * 2, pd.ad.size * 3);
	sprites.get("rightSwipe")->setTextureDimensions(-1, 1);
	sprites.get("rightSwipe")->setTexturePosition(1, 0);
	sprites.add("leftThrown", pd.thrown);
	sprites.get("leftThrown")->setColor(255, 255, 255, 0);
	sprites.get("leftThrown")->setDimensions(pd.ad.size, pd.ad.size);
	sprites.add("rightThrown", pd.thrown);
	sprites.get("rightThrown")->setColor(255, 255, 255, 0);
	sprites.get("rightThrown")->setDimensions(pd.ad.size, pd.ad.size);
	sprites.get("rightThrown")->setTextureDimensions(-1, 1);
	sprites.get("rightThrown")->setTexturePosition(1, 0);
	sprites.get("body")->canvas.position.z = 1;
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	// AOE attack
	b2CircleShape circle;
	circle.m_radius = 1.5;
	fixtureDef.shape = &circle;
	fixtureDef.userData = (void*)'S';
	b2BodyPtr->CreateFixture(&fixtureDef);
	// Swipe attack
	b2PolygonShape box;
	box.SetAsBox(.5, pd.ad.size/2, b2Vec2(0, .5), 0);
	fixtureDef.shape = &box;
	fixtureDef.userData = (void*)'f';
	b2BodyPtr->CreateFixture(&fixtureDef);
	circle.m_radius = pd.ad.size/2;
	circle.m_p.y = 1;
	fixtureDef.shape = &circle;
	fixtureDef.userData = (void*)'s';
	b2BodyPtr->CreateFixture(&fixtureDef);
}
Player::~Player() {

}
void Player::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Blood levels.
	int fireFloor = floor(blood.fire);
	int waterFloor = floor(blood.water);
	int earthFloor = floor(blood.earth);
	int airFloor = floor(blood.air);
	if (m_oldFlooredBlood.fire < fireFloor) { drainTimer.fire = 60 * 20; }
	if (m_oldFlooredBlood.water < fireFloor) { drainTimer.water = 60 * 20; }
	if (m_oldFlooredBlood.earth < fireFloor) { drainTimer.earth = 60 * 20; }
	if (m_oldFlooredBlood.air < fireFloor) { drainTimer.air = 60 * 20; }
	m_oldFlooredBlood.fire = fireFloor;
	m_oldFlooredBlood.water = fireFloor;
	m_oldFlooredBlood.earth = fireFloor;
	m_oldFlooredBlood.air = fireFloor;
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
	if (!collisionFixturePtr->IsSensor() && myFixturePtr->GetUserData() != nullptr) {
		switch ((char)myFixturePtr->GetUserData()) {
		  case 'S':
			m_aoePtrs.insert(static_cast<Object*>(
				collisionFixturePtr->GetBody()->GetUserData())
			);
		  break;
		  case 's':
			m_swipeSPtrs.insert(static_cast<Object*>(
				collisionFixturePtr->GetBody()->GetUserData())
			);
		  break;
		  case 'f':
			m_swipeFPtrs.insert(static_cast<Object*>(
				collisionFixturePtr->GetBody()->GetUserData())
			);
		  break;
		  default:
		  break;
		}
	}
}
void Player::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (!collisionFixturePtr->IsSensor() && myFixturePtr->GetUserData() != nullptr) {
		switch ((char)myFixturePtr->GetUserData()) {
		  case 'S':
			m_aoePtrs.erase(
				static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData())
			);
		  break;
		  case 's':
			m_swipeSPtrs.erase(
				static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData())
			);
		  break;
		  case 'f':
			m_swipeFPtrs.erase(
				static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData())
			);
		  break;
		  default:
		  break;
		}
	}
}
void Player::updateSprite() {
	int bodyAlpha = sprites.get("body")->canvas.color.a;
	int alpha;
	for (auto&& id : sprites.ids) {
		alpha = sprites.batch[id.second].canvas.color.a - 50;
		if (alpha < 0) { alpha = 0; }
		sprites.batch[id.second].canvas.color.a = alpha;
	}
	sprites.get("body")->canvas.color.a = bodyAlpha;
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
	Player& player = *static_cast<Player*>(actorPtr);
	actorPtr->sprites.get("body")->setColor(255, 255, 255, 255);
	for (int i = 0; i < 2; ++i) { // For each weapon
		player.m_weapons[i].selfClick = false;
		player.m_weapons[i].oldCharge = 0;
		player.m_weapons[i].charged = false;
		player.m_weapons[i].swipeTimer = 0;
		player.m_weapons[i].swipes = 0;
		player.m_weapons[i].thrown = false;
	}
}
void Player::M_Control::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	// Timers
	for (auto&& weapon : player.m_weapons) {
		if (weapon.swipeTimer == -30) { weapon.swipes = 0; }
		if (weapon.swipeTimer > -30) { --weapon.swipeTimer; }
	}
	// Face direction
	if (player.m_mousePos != player.getPosition()) {
		player.los.faceDirection = glm::normalize(player.getPosition() - player.m_mousePos);
	}
	// Attack
	float chargingMoveDampen = 0;
	int charge[2] = {
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames,
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
	};
	bool needSpinState{ false };
	for (int i = 0; i < 2; ++i) { // For each weapon
		player.m_weapons[i].swipe = false;
		player.m_weapons[i].charged =
			player.m_weapons[i].oldCharge > player.m_weapons[i].chargeTime;
		if (!charge[i]) { // Unclicked
			if (
				charge[i] < player.m_weapons[i].oldCharge && // Mouse button just unclicked
				player.m_weapons[i].swipeTimer <= 0 && // No attack rest time left
				player.m_weapons[i].swipes < player.m_weapons[i].maxSwipes // Limit not met
			) {
				if (player.m_weapons[i].selfClick) { // Last click was on self
					if (
						player.m_weapons[i].charged // Charged
						&& glm::length(player.m_mousePos - player.getPosition())
							<= player.getRadius() // Unclick on self
					) {
						// Spin
						needSpinState = true;
					} else {
						// Throw
						player.m_weapons[i].thrown = true;
						player.m_weapons[i].target = player.m_mousePos;
					}
				} else { // Last click not on self
					float bloodPlus = 0.025;
					float knockback = 10;
					if (player.m_weapons[i].charged) {
						// Heavy attack
						bloodPlus = 0.035;
						knockback = 15;
					} else {
						// Light attack
					}
					player.m_weapons[i].swipe = true;
					// Merge the swipe ptr lists for
					std::set<Object*> m_swipePtrs;
					for (auto&& ptr : player.m_swipeSPtrs) { m_swipePtrs.insert(ptr); }
					for (auto&& ptr : player.m_swipeFPtrs) { m_swipePtrs.insert(ptr); }
					// Process them
					for (auto&& hitBodyPtr : m_swipePtrs) {
						if (
							static_cast<Object*>(hitBodyPtr)->category != "actor" || // Actor
							!static_cast<Actor*>(hitBodyPtr)->movement.dodging // Not dodging
						) {
							static_cast<Object*>(hitBodyPtr)->health -=
								player.m_weapons[i].charged ? 2 : 1;
							static_cast<Object*>(hitBodyPtr)->hit = true;
							hitBodyPtr->b2BodyPtr->ApplyLinearImpulse(
								b2Vec2(
									-actorPtr->los.faceDirection.x * knockback,
									-actorPtr->los.faceDirection.y * knockback
								),
								hitBodyPtr->b2BodyPtr->GetPosition(),
								true
							);
							for (auto&& bloodPtr : player.bloodPtrs) {
								if ((*bloodPtr += bloodPlus) >= 7) { *bloodPtr = 6.99; }
							}
						}
					}
					player.m_weapons[i].swipeTimer = player.m_weapons[i].swipeRest;
					++player.m_weapons[i].swipes;
				}
			}
			player.m_weapons[i].selfClick = false;
		} else if (charge[i] == 1) { // Clicked
			if (player.m_weapons[i].thrown) {
				player.m_weapons[i].thrown = false;
			} else if (
				glm::length(player.m_mousePos - player.getPosition()) <= player.getRadius()
			) {
				player.m_weapons[i].selfClick = true;
			}
		} else { // Charging
			chargingMoveDampen = 0.75;
		}
		player.m_weapons[i].oldCharge = charge[i];
	}
	// Move direction
	player.movement.vector.x = 0;
	player.movement.vector.y = 0;
	if (player.m_uiPtr->getKeyInfo(fk::Key::W).downFrames > 0) { player.movement.vector.y += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::S).downFrames > 0) { player.movement.vector.y -= 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::D).downFrames > 0) { player.movement.vector.x += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::A).downFrames > 0) { player.movement.vector.x -= 1; }
	int shiftFrames = player.m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames;
	if (player.movement.vector.x || player.movement.vector.y) {
		float boost{ 0 };
		if (shiftFrames > 0) { boost = { 0.75 }; }
		player.movement.vector = (
			player.movement.speed + boost - chargingMoveDampen
		) * glm::normalize(player.movement.vector);
	}
	// State change
	if (player.m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames == 1 && !player.m_dodgeTimer) {
		if (shiftFrames > 0) { player.setState(new M_StunDash(*actorPtr)); }
		else { player.setState(new M_Dodge(*actorPtr)); }
	} else if (needSpinState) {
		// TODO: player.setState(new M_Spin(*actorPtr));
	} else if (player.m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames) {
		player.setState(new M_Enchant(*actorPtr));
	}
}
void Player::M_Control::updateBody() {
	State::updateBody();
}
void Player::M_Control::updateSprite() {
	Player& player = *static_cast<Player*>(actorPtr);
	if (player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames) {
		int color = player.m_weapons[0].charged ? 0 : 255;
		player.sprites.get("leftSword")->setPosition(
			player.b2BodyPtr->GetPosition().x,
			player.b2BodyPtr->GetPosition().y
		);
		player.sprites.get("leftSword")->setRotationAxis(
			player.sprites.get("leftSword")->getPosition().x,
			player.sprites.get("leftSword")->getPosition().y
		);
		player.sprites.get("leftSword")->canvas.rotationAngle =
			player.b2BodyPtr->GetAngle();
		player.sprites.get("leftSword")->setColor(color, color, color, 255);
	}
	if (player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames) {
		int color = player.m_weapons[1].charged ? 0 : 255;
		player.sprites.get("rightSword")->setPosition(
			player.b2BodyPtr->GetPosition().x,
			player.b2BodyPtr->GetPosition().y
		);
		player.sprites.get("rightSword")->setRotationAxis(
			player.sprites.get("rightSword")->getPosition().x,
			player.sprites.get("rightSword")->getPosition().y
		);
		player.sprites.get("rightSword")->canvas.rotationAngle =
			player.b2BodyPtr->GetAngle();
		player.sprites.get("rightSword")->setColor(color, color, color, 255);
	}
	if (player.m_weapons[0].swipe) {
		int color = player.m_weapons[0].charged ? 0 : 255;
		player.sprites.get("rightSwipe")->setPosition(
			player.b2BodyPtr->GetPosition().x - player.los.faceDirection.x * .5f,
			player.b2BodyPtr->GetPosition().y - player.los.faceDirection.y * .5f
		);
		player.sprites.get("rightSwipe")->setRotationAxis(
			player.sprites.get("rightSwipe")->getPosition().x,
			player.sprites.get("rightSwipe")->getPosition().y
		);
		player.sprites.get("rightSwipe")->canvas.rotationAngle =
			player.b2BodyPtr->GetAngle();
		player.sprites.get("rightSwipe")->setColor(color, color, color, 255);
	}
	if (player.m_weapons[1].swipe) {
		int color = player.m_weapons[1].charged ? 0 : 255;
		player.sprites.get("leftSwipe")->setPosition(
			player.b2BodyPtr->GetPosition().x - player.los.faceDirection.x * .5f,
			player.b2BodyPtr->GetPosition().y - player.los.faceDirection.y * .5f
		);
		player.sprites.get("leftSwipe")->setRotationAxis(
			player.sprites.get("leftSwipe")->getPosition().x,
			player.sprites.get("leftSwipe")->getPosition().y
		);
		player.sprites.get("leftSwipe")->canvas.rotationAngle =
			player.b2BodyPtr->GetAngle();
		player.sprites.get("leftSwipe")->setColor(color, color, color, 255);
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
	player.sprites.get("body")->setColor(100, 100, 255, 255);
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


void Player::M_Dodge::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	player.movement.vector.x = 0;
	player.movement.vector.y = 0;
	player.movement.dodging = true;
	player.m_dodgeTimer = 25;
	b2Filter filter = player.b2BodyPtr->GetFixtureList()->GetNext()->GetFilterData();
	filter.maskBits = 1;
	player.b2BodyPtr->GetFixtureList()->GetNext()->SetFilterData(filter);
	player.sprites.get("body")->setColor(100, 100, 255, 255);
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


void Player::M_Enchant::enter() {
	actorPtr->movement.vector.x = 0;
	actorPtr->movement.vector.y = 0;
	actorPtr->sprites.get("body")->setColor(255, 255, 255, 255);
}
void Player::M_Enchant::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	Player& player = *static_cast<Player*>(actorPtr);
	// Get sequence
	if (player.m_uiPtr->getKeyInfo(fk::Key::Q).downFrames == 1) { sequence += "q"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::W).downFrames == 1) { sequence += "w"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::E).downFrames == 1) { sequence += "e"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::A).downFrames == 1) { sequence += "a"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::S).downFrames == 1) { sequence += "s"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::D).downFrames == 1) { sequence += "d"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::Z).downFrames == 1) { sequence += "z"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::X).downFrames == 1) { sequence += "x"; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::C).downFrames == 1) { sequence += "c"; }
	// State change
	if (!player.m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames) {
		player.setState(new M_Control(*actorPtr));
	}
}
