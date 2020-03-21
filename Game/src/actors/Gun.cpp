#include "Gun.h"



Gun::Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex)
: Image(sb), w(w), bulTex(bulTex) {
	//p_sprites.emplace(sb, gunTex);
}

void Gun::fire(Body* ownerPtr, fk::Vec2 spawn, fk::Vec2 direction) {
	Body::Def bd;
	bd.position = glm::normalize(glm::vec2(direction.x, direction.y));
	bd.position /= 5;
	bd.position += spawn;
	bullets.emplace_back(ownerPtr, p_spriteBatch, w, bd, bulTex);
	bullets.back().b2Ptr->ApplyForceToCenter(direction, true);
	bullets.back().team = team;
}

inline void Gun::draw() {
	bullets.remove_if([](M_bullet& b) { return b.health < 0; });
	for (auto&& b : bullets) { b.draw(); }
}


Gun::~Gun() {
}

Gun::M_bullet::M_bullet(Body* ownerPtr, fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t)
: Image(sb), Body(w, bulDef), oldPos(bulDef.position) {
	this->ownerPtr = ownerPtr;
	type = 2;
	health = 60 + ownerPtr->charge;
	b2Ptr->SetLinearDamping(0.01);
	b2Ptr->SetBullet(true);
	std::vector<glm::vec2> line;
	line.emplace_back(0, 0);
	line.emplace_back(0, 0);
	addLineLimb(line, true);
	limbs.back().b2Ptr->SetDensity(0.001);
	limbs.back().b2Ptr->SetRestitution(0.5);
	p_sprites.emplace_back(sb, t);
	p_sprites.back().setDimensions(.04, .04);
	if (health > 120) { p_sprites.back().setColor(255, 255, 0, 255); }
}

void Gun::M_bullet::draw() {
	if (--health > 120) { p_sprites.back().setColor(255, 255, 0, 255); }
	else { p_sprites.back().setColor(255, 255, 255, 255); }
	auto position = b2Ptr->GetPosition();
	glm::vec2 p(position.x, position.y);
	p_sprites.front().makeLine(p, oldPos,0.0375);
	oldPos = p;
}

void Gun::M_bullet::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (collisionFixturePtr->IsSensor()) { return; }
	health -= ++bounces * bounces * bounces;
	auto f = bod->b2Ptr->GetPosition() - b2Ptr->GetPosition();
	glm::vec2 u = glm::normalize(glm::vec2(f.x, f.y));
	u *= 100;
	// Player related
	if (bod->type > 0 && team != bod->team) {
		int boost = 0;
		++bod->damage;
		b2Ptr->SetLinearVelocity(b2Vec2(
			health * bod->damage * u.x / 256,
			health * bod->damage * u.y / 256
		));
	// Environment
	} else if (team != bod->team) {
	}
}