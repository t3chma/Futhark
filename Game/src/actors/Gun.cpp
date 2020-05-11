#include "Gun.h"
#include "Player.h"
#include "TextBlock.h"

Gun::Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex, fk::TextSprite& t)
: sb(sb), w(w), bulTex(bulTex), text(t) {
	text.setText("`");
	text.setDepth(11);
	text[0].setColor(0, 255, 0, 255);
	text[0].setDimensions(0.3, 0.3);
}

void Gun::fire(Player* ownerPtr, fk::Vec2 spawn, fk::Vec2 direction, int level) {
	bool fire = false;
	switch (upgrade) {
	default:
		if (charge && !trigger && lastFire > 8) { fire = true; }
		break;
	case 'e':
		if (trigger && lastFire > 12 && bullets.size() == 0) { fire = true; }
		if (bullets.size()) { charge = 0; }
		break;
	case 't':
		if ((charge && !trigger && lastFire > 20) || (ammo < clipSize && lastFire > 3)) { fire = true; }
		break;
	case 'r':
		if (trigger && lastFire > 3) { fire = true; }
		break;
	case 'h':
		if (trigger && lastFire > 6) { fire = true; }
		break;
	}
	if (fire) {
		auto directwo = direction;
		auto directhree = direction;
		auto blowback = direction * 6;
		blowback.x = -blowback.x;
		blowback.y = -blowback.y;
		if (level == 1) { blowback *= 2; }
		float spread = 0.2;
		if (level == 1) { spread = 0.01; }
		Body::Def bd;
		bd.position = direction.normalized();
		bd.position /= 4;
		bd.position += spawn;
		bullets.emplace_front(ownerPtr, sb, w, bd, bulTex, level);
		int health = 0;
		switch (upgrade) {
		default:
			direction *= 1.5;
			direction.rotate(rangen.getFloat(-fk::TAU / 1024, fk::TAU / 1024));
			if (level == 1) { direction *= 5; }
			break;
		case 'm':
			direction /= 2;
			directwo /= 2;
			directhree /= 2;
			health = 12;
			if (level == 1) { health = 24; blowback *= 2; }
			bullets.front().health = health;
			for (int i = 0; i < 2; ++i) {
				directwo.rotate(spread + rangen.getFloat(-fk::TAU / 64, fk::TAU / 64));
				bullets.emplace_back(ownerPtr, sb, w, bd, bulTex, level);
				bullets.back().b2Ptr->SetLinearVelocity(directwo);
				bullets.back().health = health;
				bullets.back().team = ownerPtr->team;
				bullets.back().upgrade = upgrade;
			}
			for (int i = 0; i < 2; ++i) {
				directhree.rotate(-spread + rangen.getFloat(-fk::TAU / 64, fk::TAU / 64));
				bullets.emplace_back(ownerPtr, sb, w, bd, bulTex, level);
				bullets.back().b2Ptr->SetLinearVelocity(directhree);
				bullets.back().health = health;
				bullets.back().team = ownerPtr->team;
				bullets.back().upgrade = upgrade;
			}
			direction.rotate(rangen.getFloat(-fk::TAU / 64, fk::TAU / 64));
			if (level == 1) { direction *= 1; }
			break;
		case 'r':
			direction /= 2;
			direction.rotate(rangen.getFloat(-fk::TAU / 64, fk::TAU / 64));
			bullets.front().health = 35;
			break;
		case 'h':
			direction /= 2;
			bullets.front().limbs.back().b2Ptr->SetSensor(true);
			break;
		case 'l':
			direction.rotate(rangen.getFloat(-fk::TAU / 64, fk::TAU / 64));
			bullets.front().health = 150;
			bullets.front().sprites.back().setDimensions(1, 1);
			bullets.front().addCircleLimb(0.5);
			bullets.front().limbs.back().b2Ptr->SetSensor(true);
			direction /= 3;
			break;
		case 't':
			direction.rotate(rangen.getFloat(-fk::TAU / 128, fk::TAU / 128));
			if (level == 1) {
				bullets.front().sprites.back().setColor(0, 255, 255, 255);
			}
			else {
				bullets.front().sprites.back().setColor(0, 0, 255, 255);
			}
			direction /= 2;
			break;
		case 'e':
			direction.rotate(rangen.getFloat(-fk::TAU / 128, fk::TAU / 128));
			bullets.front().health = 120;
			if (level == 1) {
				bullets.front().sprites.back().setColor(255, 255, 0, 255);
			} else {
				bullets.front().sprites.back().setColor(255, 200, 0, 255);
			}
			direction /= 2;
			break;
		}
		ownerPtr->b2Ptr->ApplyForceToCenter(blowback, true);
		bullets.front().b2Ptr->SetLinearVelocity(direction);
		bullets.front().team = ownerPtr->team;
		bullets.front().upgrade = upgrade;
		--ammo;
		charge = 0;
		lastFire = 0;
	}
}

void Gun::setUpgrade(char u) {
	upgrade = u;
	switch (u) {
	default:
		reloadTime = 60;
		clipSize = 12;
		break;
	case 'r':
		reloadTime = 60;
		clipSize = 16;
		break;
	case 'm':
		reloadTime = 40;
		clipSize = 2;
		break;
	case 't':
		reloadTime = 25;
		clipSize = 3;
		break;
	case 'l':
		reloadTime = 60;
		clipSize = 12;
		break;
	case 'h':
		reloadTime = 1;
		clipSize = 1;
		break;
	case 'e':
		reloadTime = 10;
		clipSize = 1;
		break;
	}
	ammo = clipSize;
	reloadEdit = 0;
}

void Gun::update(fk::UserInput& ui) {
	for (auto&& b : bullets) { b.draw(); }
	for (auto&& b : bullets) { b.update(ui); }
	bullets.remove_if([](M_bullet& b) { return b.health < 0; });
	++lastFire;
	if (reloadEdit >= 0) { --reloadEdit; }
	if (reloadEdit == 0) { ammo = clipSize; --reloadEdit; }
	if (ammo == 0) { reloadEdit = reloadTime; --ammo; }
}


Gun::~Gun() { text.clearText(); }

Gun::M_bullet::M_bullet(Player* ownerPtr, fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t, int level)
	: Image(sb), Body(w, bulDef), oldPos(bulDef.position) {
	this->level = level;
	this->ownerPtr = ownerPtr;
	type = 2;
	health = 60;
	b2Ptr->SetLinearDamping(0.01);
	resistance = 0.01;
	b2Ptr->SetBullet(true);
	addCircleLimb(0.1);
	b2Filter f;
	f.categoryBits = 2;
	f.maskBits = ~2;
	limbs.back().b2Ptr->SetFilterData(f);
	limbs.back().b2Ptr->SetDensity(0.000001);
	limbs.back().b2Ptr->SetRestitution(0.7);
	sprites.emplace_back(sb, t);
	sprites.back().setDimensions(.05, .05);
	switch (level) {
	default: sprites.back().setColor(0, 0, 0, 255); break;
	case 1: sprites.back().setColor(255, 255, 255, 255); break;
	}
}

void Gun::M_bullet::draw() {
	auto position = b2Ptr->GetPosition();
	glm::vec2 p(position.x, position.y);
	switch (upgrade) {
	default: sprites.front().makeLine(oldPos, p, 0.0375); break;
	case 'e':
		if (sprites.front().getCanvasRef().color.g) {
			sprites.front().makeLine(oldPos, p, 0.0375);
		}
		else { sprites.front().setPosition(p); }
		break;
	case 'l': sprites.front().setPosition(p); break;
	}
	oldPos = p;
}

void Gun::M_bullet::update(fk::UserInput & ui) {
	switch (upgrade) {
	default: break;
	case 'h':
		b2Ptr->ApplyForceToCenter(b2Vec2(r.getFloat(-150, 150), r.getFloat(-150, 150)), true);
		break;
	case 'e':
		if (!ownerPtr->gunPtr->trigger || health == 2) {
			limbs.back().b2Ptr->SetSensor(true);
			b2Ptr->SetLinearVelocity(b2Vec2(0, 0));
			b2Ptr->SetType(b2_staticBody);
			if (level) { sprites.back().setDimensions(4, 4); }
			else { sprites.back().setDimensions(2.5, 2.5); }
			sprites.back().setColor(255, 0, 0, 100);
			if (health > 2) { health = 2; }
			if (level) { addCircleLimb(2); }
			else { addCircleLimb(1.1); }
			limbs.back().b2Ptr->SetSensor(true);
		}
		break;
	}
	--health;
	if ( level == 0 && b2Ptr->GetLinearVelocity().Length() < 5 && upgrade != 'e' ) {
		health = -1;
	}
	if (
		level == 1 && b2Ptr->GetLinearVelocity().Length() < 1
		&& health > 0 && limbs.size() < 2
	) { health = 0; }
}

void Gun::M_bullet::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	fk::Vec2 f = bod->b2Ptr->GetPosition() - b2Ptr->GetPosition();
	fk::Vec2 u = f.normalized();
	u *= 100;
	if (!collisionFixturePtr->IsSensor()) {
		if (health <= 0) {
			contactPtr->SetEnabled(false);
			for (auto&& limb : limbs) { limb.b2Ptr->SetSensor(true); }
		} else if (team != bod->team) {
			++bounces;
			// Enemy
			if (bod->type == 1) { hitEnemy(static_cast<Player*>(bod), contactPtr, u, myFixturePtr); }
			// Environment
			else if (bod->type == 0) { hitBlock(static_cast<TextBlock*>(bod), u); }
		} else if (!fresh || bounces > 0) { // Team mate
			if (!myFixturePtr->IsSensor()) {
				contactPtr->SetEnabled(false);
				for (auto&& limb : limbs) { limb.b2Ptr->SetSensor(true); }
				health = 1;
				bod->b2Ptr->SetLinearVelocity(bod->b2Ptr->GetLinearVelocity());
			}
			if (upgrade == 'e') {
				auto l = u.length();
				l *= l;
				l /= 100;
				bod->b2Ptr->SetLinearVelocity(b2Vec2(u.x / l, u.y / l));
				if (health > 2) { health = 2; }
			}
		} else {
			contactPtr->SetEnabled(false);
			bod->b2Ptr->SetLinearVelocity(bod->b2Ptr->GetLinearVelocity());
		}
		if (fresh && !myFixturePtr->IsSensor()) { fresh = false; }
	}
}

void Gun::M_bullet::hitBlock(TextBlock* tod, fk::Vec2 &u) {
	if (tod->texts.size()) {
		switch (level) {
		default: break;
		case 0: break;
		case 1:
			switch (tod->texts.front().getText()[0]) {
			default: if (upgrade != 'h') { b2Ptr->SetLinearVelocity(b2Vec2(0, 0)); }
			case 'c':
			case 'O':
			case 'o':
			case 'z':
			case 's':
			case '~': if (upgrade != 'h') { health = 1; } break;
			}
			break;
		}
		if (upgrade == 'e') {
			switch (tod->texts.front().getText()[0]) {
			default: break;
			case ':':
			case '.':
			case '-':
				tod->health -= 1;
				tod->sprites.front().getCanvasRef().color.a -= 20;
				break;
			}
			auto l = u.length();
			l *= l;
			l /= 1000;
			tod->b2Ptr->SetLinearVelocity(b2Vec2(u.x / l, u.y / l));
		}
	}
}

void Gun::M_bullet::hitEnemy(Player* pod, b2Contact* contactPtr, fk::Vec2 &u, b2Fixture* myFixturePtr) {
	if (upgrade == 'e') {
		pod->sprites.front().setColor(255, 0, 0, 255);
		pod->damage += 1;
		pod->b2Ptr->SetLinearVelocity(b2Vec2(
			pod->damage * pod->damage * u.x / 1024,
			pod->damage * pod->damage * u.y / 1024
		));
		if (health > 2) { health = 2; }
	} else if (!pod->shield) {
		if (pod->reflect < pod->reflectime) {
			fk::Vec2 reflectVel = -b2Ptr->GetLinearVelocity();
			reflectVel *= 10;
			b2Ptr->SetLinearVelocity(reflectVel);
			team = pod->team;
			health = 180;
			rebound = true;
		} else {
			if (rebound) {
				pod->shield = 0;
				pod->shealth = 0;
				pod->sprites.back().setColor(0, 0, 255, 255);
			}
			pod->sprites.front().setColor(255, 0, 0, 255);
			contactPtr->SetEnabled(false);
			for (auto&& limb : limbs) { limb.b2Ptr->SetSensor(true); }
			++pod->damage;
			if (level == 1) {
				pod->b2Ptr->SetLinearVelocity(b2Vec2(0, 0));
				pod->damage += 3;
				if (rebound) { pod->damage += 4; }
				switch (upgrade) {
				default: break;
				case 't': pod->stunned += 20; pod->immobilized += 40; break;
				}
			} else {
				if (rebound) { ++pod->damage; }
				int div = 1024;
				switch (upgrade) {
				default: break;
				case '`': div /= 2; break;
				case 't': pod->stunned += 20; break;
				}
				pod->b2Ptr->SetLinearVelocity(b2Vec2(
					pod->damage * pod->damage * u.x / div,
					pod->damage * pod->damage * u.y / div
				));
			}
			health = 1;
		}
	} else {
		contactPtr->SetEnabled(false);
		for (auto&& limb : limbs) { limb.b2Ptr->SetSensor(true); }
		if (level) { pod->b2Ptr->SetLinearVelocity(b2Vec2(0, 0)); }
		else {
			pod->b2Ptr->SetLinearVelocity(b2Vec2(
				pod->damage * pod->damage * u.x / 1024,
				pod->damage * pod->damage * u.y / 1024
			));
		}
		health = -1;
		--pod->shealth;
		pod->sprites.back().getCanvasRef().color.a -= 25;
		if (level == 1) { pod->shealth = 0; }
	}
}
