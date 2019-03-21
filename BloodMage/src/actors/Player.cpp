#include "Player.h"
#include "base/Utility.h"
#include "../SpellCatalog.h"

Player::Player(
	Map& map,
	fk::UserInput* uiPtr,
	PlayerDef& pd
) : Actor(map, pd.ad, *(new M_Control(*this)), nullptr), m_uiPtr(uiPtr) {
	// Misc
	health = 5000;
	movement.speed = 1;
	type = "player";
	enchants.melee.resize(4);
	// Graphics
	sprites.front().getCanvasRef().position.z = 1;
	sprites.emplace_back(map.dynamicObjectSprites, pd.sword);
	spritePtrs.leftSword = &sprites.back();
	sprites.emplace_back(map.dynamicObjectSprites, pd.sword);
	spritePtrs.rightSword = &sprites.back();
	sprites.emplace_back(map.dynamicObjectSprites, pd.swipe);
	spritePtrs.leftSwipe = &sprites.back();
	sprites.emplace_back(map.dynamicObjectSprites, pd.swipe);
	spritePtrs.rightSwipe = &sprites.back();
	sprites.emplace_back(map.dynamicObjectSprites, pd.ad.body);
	sprites.back().setColor(255, 255, 255, 0);
	sprites.back().setDimensions(0.1, 0.1);
	spritePtrs.leftSword->setColor(255, 255, 255, 0);
	spritePtrs.leftSword->setDimensions(pd.ad.size, pd.ad.size);
	spritePtrs.rightSword->setColor(255, 255, 255, 0);
	spritePtrs.rightSword->setDimensions(pd.ad.size, pd.ad.size);
	spritePtrs.rightSword->setTextureDimensions(-1, 1);
	spritePtrs.rightSword->setTexturePosition(1, 0);
	spritePtrs.leftSwipe->setColor(255, 255, 255, 0);
	spritePtrs.leftSwipe->setDimensions(pd.ad.size * 2, pd.ad.size * 3);
	spritePtrs.rightSwipe->setColor(255, 255, 255, 0);
	spritePtrs.rightSwipe->setDimensions(pd.ad.size * 2, pd.ad.size * 3);
	spritePtrs.rightSwipe->setTextureDimensions(-1, 1);
	spritePtrs.rightSwipe->setTexturePosition(1, 0);
	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	// AOE attack
	addCircleLimb(1, 0, 0, &fixtureDef).category = "S";
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
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).windowPosition);
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
		m_aoePtrs.insert(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
	}
}
void Player::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
	if (!collisionFixturePtr->IsSensor()) {
		m_aoePtrs.erase(static_cast<Object*>(collisionFixturePtr->GetBody()->GetUserData()));
	}
}
void Player::updateSprite() {
	int bodyAlpha = sprites.front().getCanvasRef().color.a;
	int alpha;
	for (auto&& sprite : sprites) {
		alpha = sprite.getCanvasRef().color.a - 50;
		if (alpha < 0) { alpha = 0; }
		sprite.getCanvasRef().color.a = alpha;
	}
	sprites.front().getCanvasRef().color.a = bodyAlpha;
	Actor::updateSprite();
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
	b2Vec2 test;
	// Timers
	for (auto&& weapon : player.m_weapons) {
		if (weapon.swipeTimer == -30) { weapon.swipes = 0; }
		if (weapon.swipeTimer > -30) { --weapon.swipeTimer; }
	}
	// Face direction
	if (player.m_mousePos != player.getPosition()) {
		player.los.faceDirection = glm::normalize(player.getPosition() - player.m_mousePos);
	}
	float chargingMoveDampen = 0;
	int charge[2] = {
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_LEFT).downFrames,
		player.m_uiPtr->getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames
	};
	bool needSpinState{ false };
	for (int i = 0; i < 2; ++i) { // For each weapon
		bool mouseOnSelf = glm::length(player.m_mousePos - player.getPosition()) <= player.getRadius();
		if (charge[i] < player.m_weapons[i].oldCharge) { // Unclicked
			if (player.m_weapons[i].selfClick && !mouseOnSelf) {
				// Throw
				player.m_weapons[i].thrown = true;
				player.m_weapons[i].target = player.m_mousePos;
				auto weaponOffset = i ? -player.p_radius - .1 : player.p_radius + .1;
				auto rotatedDirection = fk::rotatePoint(player.los.faceDirection, -fk::TAU / 4);
				auto raycastOrigin = player.getPosition() + glm::vec2(
					rotatedDirection.x * weaponOffset,
					rotatedDirection.y * weaponOffset
				);
				PropDef pd;
				pd.position = raycastOrigin;
				pd.linearDamping = 8;
				pd.angularDamping = 8;
				pd.bullet = true;
				pd.density = 10;
				player.m_weapons[i].thrownBodyPtr = new WarNeedle(
					player.map,
					pd,
					fk::rotatePoint(
						glm::vec2(
							-player.los.faceDirection.x * 700,
							-player.los.faceDirection.y * 700
						),
						i ? fk::TAU / 200 : -fk::TAU / 200
					),
					actorPtr
				);
			}
			player.m_weapons[i].selfClick = false;
		} else if (charge[i] == 1) { // Clicked
			if (player.m_weapons[i].thrown) { // Call back swords
				player.m_weapons[i].thrown = false;
				delete player.m_weapons[i].thrownBodyPtr;
			}
			if (mouseOnSelf) { player.m_weapons[i].selfClick = true; } // toggle self click
		}
		player.m_weapons[i].swipe = false;
		player.m_weapons[i].charged = player.m_weapons[i].oldCharge > player.m_weapons[i].chargeTime;
		bool commited = player.m_weapons[i].oldCharge > player.m_weapons[i].commitTime
			&& !player.m_weapons[i].charged;
		if (commited) { charge[i] = player.m_weapons[i].oldCharge + 1; } // Enforce commitment
		if (charge[i]) { chargingMoveDampen = 0.75; } // Commited or mouse down
		if (
			charge[i] < player.m_weapons[i].oldCharge && // Unclick or commitment done
			!player.m_weapons[i].thrown && // weapon not thrown
			player.m_weapons[i].swipeTimer <= 0 && // Attack rest over
			player.m_weapons[i].swipes < player.m_weapons[i].maxSwipes // Attacks are left
		) {
			if (mouseOnSelf) { if (player.m_weapons[i].charged) { needSpinState = true; } } // Spin
			else {
				// Attack
				float bloodPlus = 0.025;
				float knockback = 15;
				if (player.m_weapons[i].charged) {
					// Heavy attack
					bloodPlus = 0.035;
					knockback = 25;
				}
				player.m_weapons[i].swipe = true;
				auto weaponOffset = i ? -player.p_radius : player.p_radius;
				auto rotatedDirection = fk::rotatePoint(player.los.faceDirection, -fk::TAU / 4);
				auto raycastOrigin = player.getPosition() + glm::vec2(
					rotatedDirection.x * weaponOffset,
					rotatedDirection.y * weaponOffset
				);
				auto raycastTarget = glm::vec2(
					raycastOrigin.x + player.los.faceDirection.x * -1.5,
					raycastOrigin.y + player.los.faceDirection.y * -1.5
				);
				player.rayCast(
					raycastOrigin,
					raycastTarget
				);
				// Process them
				if (
					player.p_raycast.target && (
						player.p_raycast.target->category != "actor" || // Actor
						!static_cast<Actor*>(player.p_raycast.target)->movement.dodging // Not dodging
					)
				) {
					player.p_raycast.target->health -=
						player.m_weapons[i].charged ? 2 : 1;
					player.p_raycast.target->hit = true;
					player.p_raycast.target->b2Ptr->ApplyLinearImpulse(
						b2Vec2(
							-actorPtr->los.faceDirection.x * knockback,
							-actorPtr->los.faceDirection.y * knockback
						),
						b2Vec2(
							raycastOrigin.x +
							((raycastTarget.x - raycastOrigin.x) * player.p_raycast.fraction),
							raycastOrigin.y +
							((raycastTarget.y - raycastOrigin.y) * player.p_raycast.fraction)
						),
						true
					);
					for (auto&& blood : player.blood) {
						if ((blood += bloodPlus) >= 7) { blood = 6.99; }
					}
				}
				player.p_raycast.target = nullptr;
				player.m_weapons[i].swipeTimer = player.m_weapons[i].swipeRest;
				++player.m_weapons[i].swipes;
			}
		}
		player.m_weapons[i].oldCharge = charge[i];
	}
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
	// State change
	if (
		player.m_uiPtr->getKeyInfo(fk::Key::ALT_L).downFrames == 1 &&
		!player.m_dodgeTimer &&
		!charge[0] && !charge[1]
	) {
		if (shiftFrames > 0) { player.setState(new M_StunDash(*actorPtr)); }
		else { player.setState(new M_Dodge(*actorPtr)); }
	} else if (needSpinState) {
		// TODO: player.setState(new M_Spin(*actorPtr));
	} else if (player.m_uiPtr->getKeyInfo(fk::Key::SPACE).downFrames && !charge[0] && !charge[1]) {
		player.setState(new M_Enchant(*actorPtr));
	}
}
void Player::M_Control::updateBody() {
	State::updateBody();
}
void Player::M_Control::updateSprite() {
	Player& player = *static_cast<Player*>(actorPtr);
	if (player.m_weapons[0].oldCharge > player.m_weapons[0].commitTime) {
		int color = player.m_weapons[0].charged ? 0 : 255;
		player.spritePtrs.leftSword->setPosition(
			player.b2Ptr->GetPosition().x,
			player.b2Ptr->GetPosition().y
		);
		player.spritePtrs.leftSword->setRotationAxis(
			player.spritePtrs.leftSword->getPosition().x,
			player.spritePtrs.leftSword->getPosition().y
		);
		player.spritePtrs.leftSword->setRotationAngle(player.b2Ptr->GetAngle());
		player.spritePtrs.leftSword->setColor(color, color, color, 255);
	}
	if (player.m_weapons[1].oldCharge > player.m_weapons[1].commitTime) {
		int color = player.m_weapons[1].charged ? 0 : 255;
		player.spritePtrs.rightSword->setPosition(
			player.b2Ptr->GetPosition().x,
			player.b2Ptr->GetPosition().y
		);
		player.spritePtrs.rightSword->setRotationAxis(
			player.spritePtrs.rightSword->getPosition().x,
			player.spritePtrs.rightSword->getPosition().y
		);
		player.spritePtrs.rightSword->setRotationAngle(player.b2Ptr->GetAngle());
		player.spritePtrs.rightSword->setColor(color, color, color, 255);
	}
	if (player.m_weapons[0].swipe) {
		int color = player.m_weapons[0].charged ? 0 : 255;
		player.spritePtrs.rightSwipe->setPosition(
			player.b2Ptr->GetPosition().x - player.los.faceDirection.x * .5f,
			player.b2Ptr->GetPosition().y - player.los.faceDirection.y * .5f
		);
		player.spritePtrs.rightSwipe->setRotationAxis(
			player.spritePtrs.rightSwipe->getPosition().x,
			player.spritePtrs.rightSwipe->getPosition().y
		);
		player.spritePtrs.rightSwipe->setRotationAngle(
		player.b2Ptr->GetAngle());
		player.spritePtrs.rightSwipe->setColor(color, color, color, 255);
	}
	if (player.m_weapons[1].swipe) {
		int color = player.m_weapons[1].charged ? 0 : 255;
		player.spritePtrs.leftSwipe->setPosition(
			player.b2Ptr->GetPosition().x - player.los.faceDirection.x * .5f,
			player.b2Ptr->GetPosition().y - player.los.faceDirection.y * .5f
		);
		player.spritePtrs.leftSwipe->setRotationAxis(
			player.spritePtrs.leftSwipe->getPosition().x,
			player.spritePtrs.leftSwipe->getPosition().y
		);
		player.spritePtrs.leftSwipe->setRotationAngle(player.b2Ptr->GetAngle());
		player.spritePtrs.leftSwipe->setColor(color, color, color, 255);
	}
	player.sprites.back().setColor(255, 255, 255, 255);
}


void Player::M_StunDash::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	player.movement.dodging = true;
	player.movement.direction = player.m_mousePos - player.getPosition();
	if (player.movement.direction.x || player.movement.direction.y) {
		player.movement.direction = glm::normalize(player.movement.direction);
	}
	b2Filter filter = player.b2Ptr->GetFixtureList()->GetNext()->GetFilterData();
	filter.maskBits = 1;
	player.b2Ptr->GetFixtureList()->GetNext()->SetFilterData(filter);
	player.m_dodgeTimer = 40;
	player.sprites.front().setColor(100, 100, 255, 255);
}
void Player::M_StunDash::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	if (!iFrames--) {
		actorPtr->movement.dodging = false;
		b2Filter filter = actorPtr->b2Ptr->GetFixtureList()->GetNext()->GetFilterData();
		filter.maskBits = 0b1111111111111111;
		actorPtr->b2Ptr->GetFixtureList()->GetNext()->SetFilterData(filter);
		actorPtr->setState(new M_Control(*actorPtr));
	}
}
void Player::M_StunDash::updateBody() {
	if (iFrames > 10) {
		actorPtr->b2Ptr->ApplyLinearImpulse(
			b2Vec2(actorPtr->movement.direction.x * 16, actorPtr->movement.direction.y * 16),
			actorPtr->b2Ptr->GetWorldCenter(),
			true
		);
	}
}


void Player::M_Dodge::enter() {
	Player& player = *static_cast<Player*>(actorPtr);
	player.movement.direction.x = 0;
	player.movement.direction.y = 0;
	player.movement.dodging = true;
	player.m_dodgeTimer = 25;
	b2Filter filter = player.b2Ptr->GetFixtureList()->GetNext()->GetFilterData();
	filter.maskBits = 1;
	player.b2Ptr->GetFixtureList()->GetNext()->SetFilterData(filter);
	player.sprites.front().setColor(100, 100, 255, 255);
}
void Player::M_Dodge::think(std::vector<Actor*>& actorPtrs, fk::Camera * camPtr) {
	if (!iFrames--) {
		actorPtr->movement.dodging = false;
		b2Filter filter = actorPtr->b2Ptr->GetFixtureList()->GetNext()->GetFilterData();
		filter.maskBits = 0b1111111111111111;
		actorPtr->b2Ptr->GetFixtureList()->GetNext()->SetFilterData(filter);
		actorPtr->setState(new M_Control(*actorPtr));
	}
}
void Player::M_Dodge::updateBody() {
	if (iFrames > 12) {
		actorPtr->b2Ptr->ApplyLinearImpulse(
			b2Vec2(actorPtr->los.faceDirection.x * 8, actorPtr->los.faceDirection.y * 8),
			actorPtr->b2Ptr->GetWorldCenter(),
			true
		);
	}
}


void Player::M_Enchant::enter() {
	actorPtr->movement.direction.x = 0;
	actorPtr->movement.direction.y = 0;
	actorPtr->sprites.front().setColor(255, 255, 255, 255);
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
