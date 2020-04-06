#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md)
{
	setTeam(1);
	type = 1;
	addCircleLimb(0.16);
	resistance = 10;
	b2Ptr->SetLinearDamping(10);
	b2Ptr->SetBullet(true);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(200);
	limbs.back().b2Ptr->SetRestitution(0);
	sprites.emplace_back(p_spriteBatch, pd.body);
	sprites.front().setColor(0, 0, 0, 255);
	sprites.front().setDimensions(0.4, 0.4);
	sprites.front().getCanvasRef().position.z = 10;
	sprites.emplace_back(p_spriteBatch, pd.shield);
	sprites.back().setColor(255, 255, 255, 0);
	sprites.back().setDimensions(0.5, 0.5);
	sprites.back().getCanvasRef().position.z = 10;
	mouse.sprites.back().getCanvasRef().position.z = 11;
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
	if (health > 0) {
		auto move = glm::vec2(0);
		move.x = ui.getAxiInfo(joys.xMove, team);
		move.y = -ui.getAxiInfo(joys.yMove, team);
		aim.x = ui.getAxiInfo(joys.xFire, team);
		aim.y = -ui.getAxiInfo(joys.yFire, team);
		if (glm::length(move) < 5000) { move.x = 0; move.y = 0; }
		move = (move.x || move.y) ? glm::normalize(move) : move;
		move = fk::rotatePoint(move, torque);
		if (ui.getKeyInfo(fk::Key::PAD8).downFrames && team == 2) { move.y += 1; }
		if (ui.getKeyInfo(fk::Key::PAD4).downFrames && team == 2) { move.x -= 1; }
		if (ui.getKeyInfo(fk::Key::PAD6).downFrames && team == 2) { move.x += 1; }
		if (ui.getKeyInfo(fk::Key::PAD2).downFrames && team == 2) { move.y -= 1; }
		interact = ui.getJoyInfo(joys.interact, team).downFrames == 1;
		aim = fk::rotatePoint(aim.normalized(), gorque);
		float speed = 25;
		b2Vec2 myPos = b2Ptr->GetPosition();
		glm::vec2 myPosition = glm::vec2(myPos.x, myPos.y);
		auto range = (int)fk::Joy::MAXI - (int)fk::Joy::MINXI;
		bool trigger;
		if (ui.getAxiInfo(joys.fire, team) != (int)fk::Joy::MINXI) { trigger = true; }
		else { trigger = false; };
		if (
			ui.getAxiInfo(joys.shield, team) != (int)fk::Joy::MINXI
			|| ui.getKeyInfo(fk::Key::PAD5).downFrames && team == 2
		) {
			if (shealth) {
				if (shield) { shield += 1; }
				else if (reflect > recharge) { shield += 1; shealth = 10; }
			} else {
				if (reflect > recharge * 2) { shield += 1; shealth = 10; }
				else { shield = 0; stunned = 1; immobilized = 1; }
			}
		} else { shield = 0; }
		if (!shield) { ++reflect; }
		else { reflect = 0; aim.x = 0; aim.y = 0; charge = 0; }
		if (!immobilized) {
			b2Ptr->ApplyForceToCenter(b2Vec2(move.x * speed, move.y * speed), true);
		}
		if (gunPtr && !stunned) {
			if (aim.x || aim.y) {
				mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x + aim.x / 4, myPosition.y + aim.y / 4), 0);
				if (
					(gunPtr->upgrade == 'r' && trigger && gunPtr->lastFire > 4)
					|| (gunPtr->upgrade == 'h' && trigger && gunPtr->lastFire > 6)
					|| (gunPtr->charge && !trigger)
				) {
					aim.x *= 20;
					aim.y *= 20;
					int l = 0;
					if (gunPtr->charge > 60) { l = 1; }
					gunPtr->fire(this, b2Ptr->GetWorldCenter(), aim, l);
					gunPtr->charge = 0;
					gunPtr->lastFire = 0;
				}
			} else {
				mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x, myPosition.y), 0);
			}
			if (trigger && gunPtr->upgrade != 'h' && gunPtr->upgrade != 'r') {
				++gunPtr->charge;
			}
		}
	}
	if (gunPtr) { gunPtr->update(ui); }
	if (immobilized > 0) { --immobilized; }
	if (stunned > 0) { --stunned; }
	if (shield == 1) {
		b2Ptr->SetLinearDamping(resistance * 2);
	} else if (shield) {
		
	} else if (reflect == 5) {
		b2Ptr->SetLinearDamping(resistance);
	}
	timers = 0;
}
void Player::draw() {
	mouse.draw();
	mouse.setColor(0, 0, 0, 255);
	if (gunPtr->charge > 60) { mouse.setColor(255, 255, 255, 255); }
	auto position = b2Ptr->GetPosition();
	if (gunPtr) {
		gunPtr->text[0].setPosition(position.x, position.y);
		gunPtr->text[0].setRotationAxis(position.x, position.y);
		gunPtr->text[0].canvas.rotationAngle += fk::TAU / 5;
	}
	sprites.front().setPosition(position.x, position.y);
	sprites.back().setPosition(position.x, position.y);
	if (prevHealth && health < 1) { sprites.front().setColor(0, 0, 0, 100); }
	else {
		if (shield == 1) { sprites.back().setColor(255, 255, 255, 255); }
		else if (shield) {  }
		else if (reflect < reflectime) { sprites.back().setDimensions(0.7, 0.7); }
		else if (reflect < recharge) {
			sprites.back().setDimensions(0.5, 0.5);
			sprites.back().setColor(0, 0, 255, 100);
		} else {
			sprites.back().setColor(255, 255, 255, 0);
		}
		if (gunPtr && !stunned && !shield) {
			if (aim.x || aim.y) {
			}
		}
		if (sprites.front().getCanvasRef().color.r > 50) {
			sprites.front().getCanvasRef().color.r -= 50;
		}
		else { sprites.front().getCanvasRef().color.r = 0; }
		if (sprites.front().getCanvasRef().color.g > 50) {
			sprites.front().getCanvasRef().color.g -= 50;
		} else { sprites.front().getCanvasRef().color.g = 0; }
		if (sprites.front().getCanvasRef().color.b > 50) {
			sprites.front().getCanvasRef().color.b -= 50;
		} else { sprites.front().getCanvasRef().color.b = 0; }
	}
	prevHealth = health;
}

void Player::setTeam(int t) {
	team = t;
	if (gunPtr) { gunPtr->team = team; }
}
