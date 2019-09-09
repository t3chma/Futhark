#include "Dummy.h"



Dummy::Dummy(fk::SpriteBatch& sb, fk::World& world, Dummy::Def& botSpawner) : Boid(sb, world, botSpawner.bd) {
	type = (int)Type::PRACTICE;
	team = (int)Team::BANDIT;
	species = (int)Spec::DUMMY;
	addCircleLimb(0.2).b2Ptr->SetDensity(100);
	sprites.emplace_back(p_spriteBatch, botSpawner.body);
	sprites.back().setColor(255, 255, 255, 255); // white
	sprites.back().setDimensions(.4, .4);
	p_speed = 0;
}

void Dummy::p_think(fk::UserInput& ui) {  }

void Dummy::draw() {
	auto position = b2Ptr->GetPosition();
	if (p_health <= 0) { sprites.front().setColor(0, 0, 0, 255); }
	sprites.front().setPosition(position.x, position.y);
}

Dummy::~Dummy() {

}

bool Dummy::connect(Boid* bPtr) {
	bPtr->goal = glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	return glm::length(
		glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y) - bPtr->goal
	) < 0.25;
}

bool Dummy::interact(Boid* bPtr) {
	auto push = bPtr->goal - glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y);
	glm::vec2 gush = glm::vec2(10) * glm::normalize(push);
	b2Vec2 bush = b2Vec2(gush.x, gush.y);
	bPtr->b2Ptr->ApplyForceToCenter(bush, true);
	p_health -= p_maxHealth / (fk::MINUTE * 100 / 6);
	bool dead = p_health <= 0;
	return dead;
}

DumRunner::DumRunner(fk::SpriteBatch & sb, fk::World & world, Dummy::Def & dd) : Dummy(sb, world, dd) {
	pw.target = 0;
	pw.seek = 0;
	pw.snap = 0;
	p_speed = 10;
}

void DumRunner::p_think(fk::UserInput& ui) {
	
}