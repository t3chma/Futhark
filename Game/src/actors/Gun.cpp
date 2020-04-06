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

void Gun::fire(Body* ownerPtr, fk::Vec2 spawn, fk::Vec2 direction, int level) {
	auto directwo = direction;
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
		if (level == 1) { direction *= 5; }
		break;
	case 'm':
		health = 6;
		if (level == 1) { health = 8; blowback *= 2; }
		bullets.front().health = health;
		for (int i = 0; i < 2; ++i) {
			directwo.rotate(spread);
			bullets.emplace_back(ownerPtr, sb, w, bd, bulTex, level);
			bullets.back().b2Ptr->SetLinearVelocity(directwo);
			bullets.back().health = health;
			bullets.back().team = team;
			bullets.back().upgrade = upgrade;
		}
		for (int i = 0; i < 2; ++i) {
			bullets.emplace_back(ownerPtr, sb, w, bd, bulTex, level);
			bullets.back().b2Ptr->SetLinearVelocity(direction);
			bullets.back().health = health;
			bullets.back().team = team;
			bullets.back().upgrade = upgrade;
			direction.rotate(-spread);
		}
		if (level == 1) { direction *= 1; }
		break;
	case 'r':
		bullets.front().health = 30;
		break;
	case 'h':
		bullets.front().limbs.back().b2Ptr->SetSensor(true);
		break;
	case 'l':
		bullets.front().health = 150;
		bullets.front().sprites.back().setDimensions(1, 1);
		bullets.front().addCircleLimb(0.5);
		bullets.front().limbs.back().b2Ptr->SetSensor(true);
		direction /= 2;
		break;
	case 't':
		if (level == 1) {
			bullets.front().sprites.back().setColor(0, 255, 255, 255);
		} else {
			bullets.front().sprites.back().setColor(0, 0, 255, 255);
		}
		direction /= 2;
		break;
	case 'e':
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
	bullets.front().team = team;
	bullets.front().upgrade = upgrade;
}

inline void Gun::update(fk::UserInput& ui) {
	for (auto&& b : bullets) { b.draw(); }
	for (auto&& b : bullets) { b.update(ui); }
	bullets.remove_if([](M_bullet& b) { return b.health < 0; });
	++lastFire;
}


Gun::~Gun() { text.clearText(); }

Gun::M_bullet::M_bullet(Body* ownerPtr, fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t, int level)
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
	default: sprites.front().makeLine(p, oldPos, 0.0375); break;
	case 'e':
		if (sprites.front().getCanvasRef().color.g) {
			sprites.front().makeLine(p, oldPos, 0.0375);
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
		b2Ptr->ApplyForceToCenter(b2Vec2(r.getFloat(-100, 100), r.getFloat(-100, 100)), true);
		break;
	case 'e':
		if (limbs.size() < 2 && limbs.back().b2Ptr->IsSensor()) {
			if (level) { addCircleLimb(2); }
			else { addCircleLimb(1); }
			limbs.back().b2Ptr->SetSensor(true);
		}
		break;
	}
	--health;
	if (
		level == 0 && b2Ptr->GetLinearVelocity().Length() < 5
		&& (upgrade != 'e' || limbs.size() < 2)
	) { health = -1; }
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
			if (upgrade == 'e' && !myFixturePtr->IsSensor()) {
				limbs.back().b2Ptr->SetSensor(true);
				b2Ptr->SetLinearVelocity(b2Vec2(0,0));
				b2Ptr->SetType(b2_staticBody);
				if (level) { sprites.back().setDimensions(4, 4); }
				else { sprites.back().setDimensions(2, 2); }
				sprites.back().setColor(255, 0, 0, 100);
				b2Ptr->SetLinearDamping(1000);
				health = 3;
			} else {
				++bounces;
				// Enemy
				if (bod->type == 1) { hitEnemy(static_cast<Player*>(bod), contactPtr, u, myFixturePtr); }
				// Environment
				else { hitBlock(static_cast<TextBlock*>(bod), u); }
			}
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
			}
		} else {
			contactPtr->SetEnabled(false);
			bod->b2Ptr->SetLinearVelocity(bod->b2Ptr->GetLinearVelocity());
		}
		if (fresh && !myFixturePtr->IsSensor()) { fresh = false; }
	}
}

void Gun::M_bullet::hitBlock(TextBlock* tod, fk::Vec2 &u) {
	switch (level) {
	default: break;
	case 0: break;
	case 1:
		switch (tod->texts.front().getText()[0]) {
		default: if (upgrade != 'h') { b2Ptr->SetLinearVelocity(b2Vec2(0, 0)); }
		case '`':
		case 'm':
		case 'r':
		case 'l':
		case 'c':
		case 'O':
		case 'o':
		case 'z':
		case 's':
		case 't':
		case 'h':
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

void Gun::M_bullet::hitEnemy(Player* pod, b2Contact* contactPtr, fk::Vec2 &u, b2Fixture* myFixturePtr) {
	if (upgrade == 'e') {
		pod->sprites.front().setColor(255, 0, 0, 255);
		pod->damage += 1;
		pod->b2Ptr->SetLinearVelocity(b2Vec2(
			pod->damage * pod->damage * u.x / 1024,
			pod->damage * pod->damage * u.y / 1024
		));
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
				pod->damage += 1;
				if (rebound) { pod->damage += 2; }
				switch (upgrade) {
				default: break;
				case 't': pod->stunned = 30; pod->immobilized = 30; break;
				}
			} else {
				if (rebound) { ++pod->damage; }
				int div = 1024;
				switch (upgrade) {
				default: break;
				case '`': div /= 2; break;
				case 't': pod->stunned = 30; break;
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
