#include "Gun.h"



Gun::Gun(fk::SpriteBatch& sb, fk::World& w, fk::Texture& bulTex)
: Image(sb), w(w), bulTex(bulTex) {
	//p_sprites.emplace(sb, gunTex);
}

void Gun::fire(fk::Vec2 spawn, fk::Vec2 target) {
	Body::Def bd;
	fk::Vec2 direction = target - spawn;
	direction = glm::normalize(glm::vec2(direction));
	bd.position = spawn + direction / 5;
	bullets.emplace_back(p_spriteBatch, w, bd, bulTex);
	bullets.back().b2Ptr->ApplyForceToCenter(direction * 1000, true);
	bullets.back().team = team;
}

inline void Gun::draw() {
	bullets.remove_if([](M_bullet& b) { return b.health < 0; });
	for (auto&& b : bullets) { b.draw(); }
}


Gun::~Gun() {
}

Gun::M_bullet::M_bullet(fk::SpriteBatch& sb, fk::World& w, Body::Def& bulDef, fk::Texture& t)
: Image(sb), Body(w, bulDef) {
	type = 2;
	health = 60;
	b2Ptr->SetLinearDamping(1);
	b2Ptr->SetBullet(true);
	std::vector<glm::vec2> line;
	line.emplace_back(0, 0);
	line.emplace_back(0, 0);
	addLineLimb(line, true);
	limbs.back().b2Ptr->SetDensity(100);
	limbs.back().b2Ptr->SetRestitution(1);
	p_sprites.emplace_back(sb, t);
	p_sprites.back().setDimensions(.04, .04);
}

void Gun::M_bullet::draw() {
	--health;
	auto position = b2Ptr->GetPosition();
	p_sprites.front().setPosition(position.x, position.y);
}

void Gun::M_bullet::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	health -= ++bounces * bounces * bounces;
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	auto f = bod->b2Ptr->GetPosition() - b2Ptr->GetPosition();
	glm::vec2 u = glm::normalize(glm::vec2(f.x, f.y));
	u *= 20;
	// Player related
	if (bod->type > 0 && team != bod->team) {
		bod->b2Ptr->SetLinearVelocity(b2Vec2(u.x, u.y));
		b2Ptr->SetLinearVelocity(b2Vec2(0, 0));
	} else if (team != bod->team) {
		b2Ptr->SetLinearVelocity(b2Vec2(u.x / 8, u.y / 8));
	}
}