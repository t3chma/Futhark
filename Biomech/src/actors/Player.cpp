#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md), hudFont(pd.hudFont)
{
	type = (int)Type::PLAYER;
	team = (int)Team::PLAYER;
	species = (int)Spec::PLAYER;
	addCircleLimb(0.2);
	b2Ptr->SetLinearDamping(10);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(10);
	limbs.back().b2Ptr->SetRestitution(0);
	botSpawner = pd.botSpawner;
	botSpawner.basePtr = this;
	tempHUD.reserve(2);
	float size = 100;
	for (int i = 0; i < 2; ++i) {
		tempHUD.push_back(hudFont.generateCharSprites("0", textBatch, glm::vec2(size, size / 2)));
		tempHUD.back().setPosition(glm::vec2(i*size * 2 + size / 2, size / 2));
	}
	p_sprites.emplace_back(p_spriteBatch, pd.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(0.4, 0.4);
}
Player::~Player() {

}
void Player::update(fk::UserInput& ui) {
	mouse.update(ui);
	for (auto&& squad : hive.squads) { for (auto&& bot : squad.bots) { bot.update(ui); } }
	if (ui.getKeyInfo(fk::Key::Q).downFrames == 1) { hive.squad = 1; }
	if (ui.getKeyInfo(fk::Key::E).downFrames == 1) { hive.squad = 2; }
	auto direction = glm::vec2(0);
	if (ui.getKeyInfo(right).downFrames > 1) { direction.x += 1; }
	if (ui.getKeyInfo(left).downFrames > 1) { direction.x -= 1; }
	if (ui.getKeyInfo(up).downFrames > 1) { direction.y += 1; }
	if (ui.getKeyInfo(down).downFrames > 1) { direction.y -= 1; }
	auto normalDir = (direction.x || direction.y) ? glm::normalize(direction) : direction;
	float speed = 15;
	b2Vec2 myPos = b2Ptr->GetPosition();
	glm::vec2 myPosition = glm::vec2(myPos.x, myPos.y);
	if (ui.getKeyInfo(fk::Key::SHIFT_L).downFrames == 1 || ui.getKeyInfo(fk::Key::SHIFT_R).downFrames == 1) {
		aiming = !aiming;
	}
	if (aiming) {
		speed /= 2;
	} else if (ui.getKeyInfo(fk::Key::CTRL_L).downFrames || ui.getKeyInfo(fk::Key::CTRL_R).downFrames) {
		if (ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1 && hive.isActive()) {
			auto text = std::to_string(hive.squadSize() + 1);
			tempHUD[hive.squad - 1].setText(text);
			botSpawner.db.bd.position = fk::Vec2(myPos.x, myPos.y);
			hive.addBot(p_spriteBatch, world, botSpawner);
		}
		if (ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1 && hive.isActive()) {
			auto text = std::to_string(hive.squadSize() - 1);
			tempHUD[hive.squad - 1].setText(text);
			hive.removeBot();
		}
		if (ui.getKeyInfo(fk::Key::MOUSE_MIDDLE).downFrames == 1) {
			hive.toggleActivation();
			std::string text;
			if (hive.isActive()) { text = std::to_string(hive.squadSize()); }
			else { text = "X"; }
			tempHUD[hive.squad - 1].setText(text);
		}
	} else {
		int priority{ -1 };
		static int mouseLeft{ 0 };
		int left{ ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames };
		if (mouseLeft && left == 0) {
			mouse.click(priority = 0);
			b2Vec2 mousePos = mouse.b2Ptr->GetPosition();
			glm::vec2 mousePosition = glm::vec2(mousePos.x, mousePos.y);
			hive.launchSquad(mousePosition, myPosition);
		}
		mouseLeft = ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames;
		static int mouseRight{ 0 };
		int right{ ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames };
		if (mouseRight && right == 0) {
			mouse.click(priority = 1);
			b2Vec2 mousePos = mouse.b2Ptr->GetPosition();
			glm::vec2 mousePosition = glm::vec2(mousePos.x, mousePos.y);
			hive.launchSquad(mousePosition, myPosition);
		}
		mouseRight = ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames;
		if (priority == 0) { hive.setPrimaryGoal(mouse.goal.location, mouse.goal.bodyPtr); }
		else if (priority == 1) { hive.setSecondaryGoal(mouse.goal.location, mouse.goal.bodyPtr); }
	}
	b2Ptr->ApplyForceToCenter(b2Vec2(normalDir.x * speed, normalDir.y * speed), true);
}
void Player::draw() {
	mouse.draw();
	for (auto&& squad : hive.squads) { for (auto&& bot : squad.bots) { bot.draw(); } }
	auto position = b2Ptr->GetPosition();
	p_sprites.front().setPosition(position.x, position.y);
}

bool Player::connect(Boid* bPtr) {
	bPtr->goal = glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	return glm::length(
		glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y) - bPtr->goal
	) < 0.1;
}

bool Player::interact(Boid* bPtr) {
	static fk::Random rangen;
	bPtr->b2Ptr->ApplyForceToCenter(b2Vec2(rangen.getFloat(-50, 50), rangen.getFloat(-50, 50)), true);
	return false;
}