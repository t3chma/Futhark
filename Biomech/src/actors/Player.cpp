#include "Player.h"
#include "../spectrals/Equipment.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& tb, fk::Camera& c, fk::World& w, Player::Def pd)
	: Boid(sb, w, pd.bd), mouse(sb, world, pd.md), hudFont(pd.hudFont), camera(c)
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
	tempHUD.reserve(3);
	float size = 100;
	for (int i = 0; i < 2; ++i) {
		tempHUD.push_back(hudFont.generateCharSprites("0", tb, glm::vec2(size, size / 2)));
		tempHUD.back().setPosition(glm::vec2(i*size * 2 + size / 2, size / 2));
	}
	tempHUD.push_back(hudFont.generateCharSprites("100", tb, glm::vec2(size, size / 2)));
	tempHUD.back().setPosition(glm::vec2(2 * size * 2 + size / 2, size / 2));
	sprites.emplace_back(p_spriteBatch, pd.body);
	sprites.back().setColor(255, 255, 255, 255); // white
	sprites.back().setDimensions(0.4, 0.4);
	pw.studder = 0;
	pw.alignment = 0;
	pw.seperation = 0;
	pw.cohesion = 0;
	pw.seek = 0;
	pw.target = 0;
	pw.snap = 0;
	pw.aversion = 0;
	goals[0].resolved = false;
	auto position = b2Ptr->GetPosition();
}

Player::~Player() {

}

inline void Player::primeGadget(int option) {
	if (p_gadgetPtr) {
		Target t;
		if (mouse.goal.bodyPtr && mouse.goal.bodyPtr->species != (int)Spec::MOUSE) { t.bodyPtr = mouse.goal.bodyPtr; }
		t.location = mouse.goal.location;
		p_gadgetPtr->prime(option, this, t);
	}
}

inline void Player::primeGun(int option) {
	if (p_gunPtr) {
		Target t;
		t.bodyPtr = mouse.goal.bodyPtr;
		t.location = mouse.goal.location;
		p_gunPtr->prime(option, this, t);
	}
}

void Player::p_think(fk::UserInput& ui) {
	auto position = b2Ptr->GetPosition();
	p_looking = glm::vec2(mouse.b2Ptr->GetPosition().x, mouse.b2Ptr->GetPosition().y);
	for (auto&& squad : hive.squads) { for (auto&& bot : squad.bots) { bot.think(ui); } }
	if (ui.getMouseInfo(0).wheel == 1) { camera.zoomIn(1.2); }
	if (ui.getMouseInfo(0).wheel == -1) { camera.zoomOut(1.2); }
	if (ui.getKeyInfo(fk::Key::Q).downFrames == 1) { hive.squad = 1; }
	if (ui.getKeyInfo(fk::Key::E).downFrames == 1) { hive.squad = 2; }
	auto direction = glm::vec2(0);
	if (ui.getKeyInfo(right).downFrames > 1) { direction.x += 1; }
	if (ui.getKeyInfo(left).downFrames > 1) { direction.x -= 1; }
	if (ui.getKeyInfo(up).downFrames > 1) { direction.y += 1; }
	if (ui.getKeyInfo(down).downFrames > 1) { direction.y -= 1; }
	p_direction = (direction.x || direction.y) ? glm::normalize(direction) : direction;
	float speed = p_speed;
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
			hive.launchSquad(p_looking, myPosition);
		}
		mouseLeft = ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames;
		static int mouseRight{ 0 };
		int right{ ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames };
		if (mouseRight && right == 0) {
			mouse.click(priority = 1);
			hive.launchSquad(p_looking, myPosition);
		}
		mouseRight = ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames;
		if (priority == 0) { hive.setPrimaryGoal(mouse.goal.location, mouse.goal.bodyPtr); }
		else if (priority == 1) { hive.setSecondaryGoal(mouse.goal.location, mouse.goal.bodyPtr); }
	}
	int static downFrames{ 0 };
	if (ui.getKeyInfo(fk::Key::SPACE).downFrames == 0 && downFrames) {
		releaseGadget();
	}
	else if (ui.getKeyInfo(fk::Key::SPACE).downFrames == 1) {
		primeGadget(0);
	}
	downFrames = ui.getKeyInfo(fk::Key::SPACE).downFrames;
	std::string text = std::to_string(p_health);
	tempHUD[2].setText(text);
	speed *= statEffects.speedBoost;
	statEffects.speedBoost = 1.0;
	b2Ptr->ApplyForceToCenter(b2Vec2(p_direction.x * speed, p_direction.y * speed), true);
	if (p_gunPtr || p_gadgetPtr) {
		Target t;
		if (mouse.goal.bodyPtr && mouse.goal.bodyPtr->species != (int)Spec::MOUSE) { t.bodyPtr = mouse.goal.bodyPtr; }
		t.location = mouse.goal.location;
		if (p_gunPtr) { p_gunPtr->update(ui, t); }
		if (p_gadgetPtr) { p_gadgetPtr->update(ui, t); }
	}
	mouse.think(ui);
	glm::vec2 mousePos = camera.getWorldCoordinates(ui.getMouseInfo(0).windowPosition);
	mouse.b2Ptr->SetTransform(b2Vec2(mousePos.x, mousePos.y), 0);
}

void Player::draw() {
	mouse.draw();
	for (auto&& squad : hive.squads) { for (auto&& bot : squad.bots) { bot.draw(); } }
	auto position = b2Ptr->GetPosition();
	sprites.front().setPosition(position.x, position.y);
	if (p_gunPtr) { p_gunPtr->draw(); }
	if (p_gadgetPtr) { p_gadgetPtr->draw(); }
}

bool Player::connect(Boid* bPtr) {
	bPtr->goal = glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	return glm::length(
		glm::vec2(bPtr->b2Ptr->GetPosition().x, bPtr->b2Ptr->GetPosition().y) - bPtr->goal
	) < 0.4;
}

bool Player::interact(Boid* bPtr) {
	static fk::Random rangen;
	glm::vec2 force = glm::vec2(25) * glm::normalize(
		glm::vec2(rangen.getFloat(-0.3, 0.3), rangen.getFloat(-0.3, 0.3))
		+ p_looking
		- glm::vec2(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y)
	);
	bPtr->b2Ptr->ApplyForceToCenter(b2Vec2(force.x, force.y), false);
	return false;
}