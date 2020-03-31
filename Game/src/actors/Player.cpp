#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md)
{
	setTeam(1);
	type = 1;
	addCircleLimb(0.2);
	resistance = 10;
	b2Ptr->SetLinearDamping(10);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(200);
	limbs.back().b2Ptr->SetRestitution(0);
	sprites.emplace_back(p_spriteBatch, pd.body);
	sprites.back().setColor(0, 0, 0, 255); // white
	sprites.back().setDimensions(0.4, 0.4);
	sprites.back().getCanvasRef().position.z = 10;
	fk::TextSprite t = pd.hudFont->generateCharSprites(" ", sb, glm::vec2(0.5));
	t.setDepth(5);
	gunPtr = new Gun(sb, world, pd.md.body, t);
	gunPtr->team = team;
}
Player::~Player() {
	delete gunPtr;
}
void Player::update(fk::UserInput& ui) {
	mouse.update(ui);
	mouse.setColor(0, 0, 0, 0);
	if (health > 0) {
		auto move = glm::vec2(0);
		auto aim = glm::vec2(0);
		move.x = ui.getAxiInfo(joys.xMove, team);
		move.y = -ui.getAxiInfo(joys.yMove, team);
		aim.x = ui.getAxiInfo(joys.xFire, team);
		aim.y = -ui.getAxiInfo(joys.yFire, team);
		if (glm::length(move) < 5000) { move.x = 0; move.y = 0; }
		move = (move.x || move.y) ? glm::normalize(move) : move;
		move = fk::rotatePoint(move, torque);
		aim = (aim.x || aim.y) ? glm::normalize(aim) : aim;
		aim = fk::rotatePoint(aim, gorque);
		float speed = 25;
		b2Vec2 myPos = b2Ptr->GetPosition();
		glm::vec2 myPosition = glm::vec2(myPos.x, myPos.y);
		b2Ptr->ApplyForceToCenter(b2Vec2(move.x * speed, move.y * speed), true);
		auto range = (int)fk::Joy::MAXI - (int)fk::Joy::MINXI;
		bool trigger;
		if (ui.getAxiInfo(joys.fire, team) != (int)fk::Joy::MINXI) { trigger = true; }
		else { trigger = false; };
		if (gunPtr) {
			if (aim.x || aim.y) {
				mouse.setColor(0, 0, 0, 255);
				mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x + aim.x / 4, myPosition.y + aim.y / 4), 0);
				if (trigger != oldTrigger) {
					if (!trigger) {
						aim.x *= 1000;
						aim.y *= 1000;
						int l = 0;
						if (gunPtr->charge > 60) { aim *= 5; l = 1; }
						mouse.setColor(255, 0, 0, 255);
						gunPtr->fire(this, b2Ptr->GetWorldCenter(), aim, l);
						gunPtr->charge = 0;
					}
				}
			} else {
				mouse.setColor(0, 0, 0, 0);
			}
			if (trigger) { ++gunPtr->charge; }
		}
		oldTrigger = trigger;
	}
	if (gunPtr) { gunPtr->update(ui); }
}
void Player::draw() {
	mouse.draw();
	auto position = b2Ptr->GetPosition();
	if (gunPtr) {
		gunPtr->text[0].setPosition(position.x, position.y);
		gunPtr->text[0].setRotationAxis(position.x, position.y);
		gunPtr->text[0].canvas.rotationAngle += fk::TAU / 5;
	}
	sprites.front().setPosition(position.x, position.y);
	if (prevHealth && health < 1) { sprites.front().setColor(0, 0, 0, 100); }
	else {
		if (gunPtr->charge > 60) { mouse.setColor(255, 255, 255, 255); }
		if (sprites.front().getCanvasRef().color.r > 50) {
			sprites.front().getCanvasRef().color.r -= 50;
		}
		else { sprites.front().getCanvasRef().color.r = 0; }
		if (sprites.front().getCanvasRef().color.g > 50) {
			sprites.front().getCanvasRef().color.g -= 50;
		}
		else { sprites.front().getCanvasRef().color.g = 0; }
		if (sprites.front().getCanvasRef().color.b > 50) {
			sprites.front().getCanvasRef().color.b -= 50;
		}
		else { sprites.front().getCanvasRef().color.b = 0; }
	}
	prevHealth = health;
}

void Player::setTeam(int t) {
	team = t;
	if (gunPtr) { gunPtr->team = team; }
}
