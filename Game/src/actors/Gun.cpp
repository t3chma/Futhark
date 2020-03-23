#include "Gun.h"
#include "Player.h"


Gun::Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex, fk::TextSprite& t)
: sb(sb), w(w), bulTex(bulTex), text(t) {
}

void Gun::fire(Body* ownerPtr, fk::Vec2 spawn, fk::Vec2 direction, int level) {
	Body::Def bd;
	bd.position = direction.normalized();
	bd.position /= 5;
	bd.position += spawn;
	bullets.emplace_back(ownerPtr, sb, w, bd, bulTex, level);
	bullets.back().b2Ptr->ApplyForceToCenter(direction, true);
	bullets.back().team = team;
}

inline void Gun::update(fk::UserInput& ui) {
	bullets.remove_if([](M_bullet& b) { return b.health < 0; });
	for (auto&& b : bullets) { b.draw(); }
	for (auto&& b : bullets) { b.update(ui); }
}


Gun::~Gun() {
}

Gun::M_bullet::M_bullet(Body* ownerPtr, fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t, int level)
	: Image(sb), Body(w, bulDef), oldPos(bulDef.position) {
	this->level = level;
	this->ownerPtr = ownerPtr;
	type = 2;
	health = 60 + charge;
	b2Ptr->SetLinearDamping(0.01);
	resistance = 0.01;
	b2Ptr->SetBullet(true);
	addCircleLimb(0.01);
	limbs.back().b2Ptr->SetDensity(0.000001);
	limbs.back().b2Ptr->SetRestitution(0.5);
	sprites.emplace_back(sb, t);
	sprites.back().setDimensions(.04, .04);
	switch (level) {
	default: sprites.back().setColor(0, 0, 0, 255); break;
	case 1: sprites.back().setColor(255, 255, 255, 255); break;
	}
}

void Gun::M_bullet::draw() {
	switch (level) {
	default: sprites.back().setColor(0, 0, 0, 255); break;
	case 1: sprites.back().setColor(255, 255, 255, 255); break;
	}
	auto position = b2Ptr->GetPosition();
	glm::vec2 p(position.x, position.y);
	sprites.front().makeLine(p, oldPos,0.0375);
	oldPos = p;
}

void Gun::M_bullet::update(fk::UserInput & ui) {
	--health;
}

void Gun::M_bullet::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (collisionFixturePtr->IsSensor() || myFixturePtr->IsSensor()) { return; }
	++bounces;
	auto f = bod->b2Ptr->GetPosition() - b2Ptr->GetPosition();
	glm::vec2 u = glm::normalize(glm::vec2(f.x, f.y));
	u *= 100;
	// Player related
	if (bod->type > 0 && team != bod->team) {
		int boost = 0;
		++bod->damage;
		if (bod->type == 1) {
			Player* pod = static_cast<Player*>(bod);
			pod->sprites.back().setColor(255, 0, 0, 255);
		}
		if (level == 1) {
			bod->damage += 9;
			b2Ptr->SetLinearVelocity(b2Vec2(0, 0));
		} else {
			b2Ptr->SetLinearVelocity(b2Vec2(
				bod->damage * u.x / 128,
				bod->damage * u.y / 128
			));
		}
		health = 0;
	// Environment
	} else if (team != bod->team) {
		switch (level) {
		default: break;
		case 1: b2Ptr->SetLinearVelocity(b2Vec2(0, 0)); health = 0; break;
		}
	}
}