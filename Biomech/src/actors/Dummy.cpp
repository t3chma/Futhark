#include "Dummy.h"



Dummy::Dummy(fk::SpriteBatch& sb, fk::World& world, Dummy::Def& botSpawner) : Boid(sb, world, botSpawner.bd) {
	type = (int)Type::PRACTICE;
	team = (int)Team::BANDIT;
	species = (int)Spec::DUMMY;
	addCircleLimb(0.2).b2Ptr->SetDensity(100);
	p_sprites.emplace_back(p_spriteBatch, botSpawner.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(.4, .4);
	goals.resize(1);
}

void Dummy::update(fk::UserInput& ui) {
}

void Dummy::draw() {
	auto position = b2Ptr->GetPosition();
	if (p_health < 1) { p_sprites.front().setColor(0, 0, 0, 255); }
	p_sprites.front().setPosition(position.x, position.y);
}

Dummy::~Dummy() {

}

bool Dummy::connect(Boid* bPtr) {
	bPtr->goal = glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	return glm::length(
		glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y) - bPtr->goal
	) < 0.3;
}

bool Dummy::interact(Boid* bPtr) {
	auto push = bPtr->goal - glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y);
	glm::vec2 gush = glm::vec2(10) * glm::normalize(push);
	b2Vec2 bush = b2Vec2(gush.x, gush.y);
	bPtr->b2Ptr->ApplyForceToCenter(bush, true);
	bool dead = --p_health < 1;
	return dead;
}

void DumRunner::update(fk::UserInput& ui) {
	if (p_health > 0) { Boid::update(ui); }
}