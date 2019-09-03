#include "Bot.h"



Bot::Bot(fk::SpriteBatch& sb, fk::World& world, Bot::Def& bd) : Boid(sb, world, bd.db) {
	type = (int)Type::SWARM;
	team = (int)Team::PLAYER;
	species = (int)Spec::BOT;
	addCircleLimb(0.03);
	b2Ptr->SetLinearDamping(10);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(10);
	limbs.back().b2Ptr->SetRestitution(0);
	p_sprites.emplace_back(p_spriteBatch, bd.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(0.06, 0.04);
	p_sprites.back().getCanvasRef().position.z = 100;
	goals.resize(3);
	goals[2].bodyPtr = bd.basePtr;
	goals[2].location = glm::vec2(bd.basePtr->b2Ptr->GetPosition().x, bd.basePtr->b2Ptr->GetPosition().y);
	goals[2].resolved = false;
}


Bot::~Bot() {

}

void Bot::update(fk::UserInput& ui) { Boid::update(ui); }

void Bot::draw() {
	auto position = b2Ptr->GetPosition();
	auto p = glm::vec2(position.x, position.y);
	p_sprites.front().setPosition(p);
	p_sprites.front().setRotationAngle(b2Ptr->GetAngle());
	p_sprites.front().setRotationAxis(p);
}

size_t HiveMind::squadSize(){
	return squads[squad - 1].bots.size();
}

bool HiveMind::isActive() {
	return squads[squad - 1].active;
}

void HiveMind::toggleActivation() {
	squads[squad - 1].active = !squads[squad - 1].active;
}

void HiveMind::removeBot() {
	squads[squad - 1].bots.pop_back();
}

void HiveMind::addBot(fk::SpriteBatch& sb, fk::World& w, Bot::Def bd) {
	squads[squad - 1].bots.emplace_back(sb, w, bd);
	squads[squad - 1].bots.back().squad = squad;
}

void HiveMind::setPrimaryGoal(
	glm::vec2& location,
	Body* bodyPtr
) {
	for (auto&& bot : squads[squad - 1].bots) {
		bot.goals[0].location = location;
		bot.goals[0].bodyPtr = bodyPtr;
		bot.goals[0].resolved = false;
	}
}

void HiveMind::setSecondaryGoal(
	glm::vec2& location,
	Body* bodyPtr
) {
	for (auto&& bot : squads[squad - 1].bots) {
		bot.goals[1].location = location;
		bot.goals[1].bodyPtr = bodyPtr;
		bot.goals[1].resolved = false;
	}
}

void HiveMind::launchSquad(glm::vec2& target, glm::vec2& launcher) {
	for (auto&& bot : squads[squad - 1].bots) {
		glm::vec2 pos = glm::vec2(bot.b2Ptr->GetPosition().x, bot.b2Ptr->GetPosition().y);
		auto dv = target - launcher;
		auto l = glm::length(pos - launcher);
		dv = glm::vec2(300) * glm::normalize(target - launcher) / (l * l * l);
		dv = glm::length(dv) > 600 ? glm::vec2(600) * glm::normalize(dv) : dv;
		bot.b2Ptr->ApplyForceToCenter(b2Vec2(dv.x, dv.y), true);
	}
}

HiveMind::HiveMind() {
	squads.resize(9);
}
