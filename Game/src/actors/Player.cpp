#include "Player.h"
#include "TextBlock.h"

Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md)
{
	setTeam(1);
	type = 1;
	addCircleLimb(0.16);
	resistance = 10;
	b2Ptr->SetLinearDamping(10);
	b2Ptr->SetBullet(true);
	limbs.back().category = "b";
	limbs.back().b2Ptr->SetDensity(200);
	limbs.back().b2Ptr->SetRestitution(0);
	sprites.emplace_back(p_spriteBatch, pd.body);
	sprites.front().setColor(255, 255, 255, 255);
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
inline float Player::ReportFixture(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float fraction) {
	los = fixture;
	return 0;
}
void Player::update(fk::UserInput& ui) {
	mouse.update(ui);
	if (health > 0) {
		move = glm::vec2(0);
		if (ai) { GetAIInput(); }
		else { getPlayerInput(ui); }
		move = move.normalized();
		move.rotate(torque);
		aim = aim.normalized();
		aim.rotate(gorque);
		float speed = 25;
		if (dodge == dodgeFrames) { speed *= 32; }
		else if (immobilized || (dodge < dodgeFrames && dodge > 0)) { speed = 0; }
		b2Vec2 myPos = b2Ptr->GetPosition();
		auto myPosition = fk::Vec2(myPos.x, myPos.y);
		auto range = (int)fk::Joy::MAXI - (int)fk::Joy::MINXI;
		if (!shield) { ++reflect; }
		else { reflect = 0; aim.x = 0; aim.y = 0; charge = 0; }
		b2Ptr->ApplyForceToCenter(b2Vec2(move.x * speed, move.y * speed), true);
		if (gunPtr && gunPtr->ammo > 0 && !stunned) {
			if (aim.x || aim.y) {
				mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x + aim.x / 4, myPosition.y + aim.y / 4), 0);
				aim.x *= 20;
				aim.y *= 20;
				int l = 0;
				if (gunPtr->charge > 60) { l = 1; }
				gunPtr->fire(this, b2Ptr->GetWorldCenter(), aim, l);
			} else {
				mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x, myPosition.y), 0);
			}
			if (gunPtr->trigger && gunPtr->reloadEdit < 1 && gunPtr->upgrade != 'h' && gunPtr->upgrade != 'r') {
				++gunPtr->charge;
			}
		} else {
			mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x, myPosition.y), 0);
		}
	}
	if (gunPtr) { gunPtr->update(ui); }
	if (dodge > 0) { --dodge; }
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

void Player::GetAIInput() {
	bool p = false;
	Player* pPtr{ nullptr };
	float distance = 0;
	for (auto&& player : players) {
		fk::Vec2 v = player->b2Ptr->GetPosition() - b2Ptr->GetPosition();
		float d = v.length();
		if (pPtr == nullptr || (d < distance && pPtr->health > 0)) { pPtr = player; distance = d; }
	}
	if (pPtr) {
		if (gunPtr) {
			fk::Vec2 d = pPtr->b2Ptr->GetPosition() - b2Ptr->GetPosition();
			aim = d;
			float farRange = 0;
			float shortRange = 0;
			int f = 0;
			switch (gunPtr->upgrade) {
			default:
				if (rangen.getInt(0, 40) == 0) { gunPtr->trigger = !gunPtr->trigger; }
				farRange = 5;
				shortRange = 1;
				break;
			case 'm':
				farRange = 1;
				shortRange = 0;
				if (d.length() > farRange) { f = 80; }
				if (rangen.getInt(0, f) == 0) { gunPtr->trigger = !gunPtr->trigger; }
				break;
			case 'r':
				if (rangen.getInt(0, 40) == 0) { gunPtr->trigger = !gunPtr->trigger; }
				farRange = 3;
				break;
			case 't':
				if (rangen.getInt(0, 30) == 0) { gunPtr->trigger = !gunPtr->trigger; }
				farRange = 4;
				break;
			case 'h':
				if (rangen.getInt(0, 300) > 0) { gunPtr->trigger = true; }
				else { gunPtr->trigger = false; }
				farRange = 2;
				break;
			case 'l':
				if (rangen.getInt(0, 16) == 0) { gunPtr->trigger = !gunPtr->trigger; }
				farRange = 5;
				break;
			case 'e':
				if (rangen.getInt(0, 8) == 0) { gunPtr->trigger = !gunPtr->trigger; }
				farRange = 5;
				break;
			}
			auto nd = d.normalized();
			nd.rotate(fk::TAU / 8 * rangen.getInt(-1, 1));
			if (d.length() < shortRange) { move -= nd; }
			else if (d.length() > farRange) { move += nd; }
			else { nd.rotate(fk::TAU / 8 * rangen.getInt(-1, 1)); move += nd * rangen.getInt(0, 1); }
		} else {

		}
	} else {
		gunPtr->trigger = false;
		if (gunPtr) {
			if (gunPtr->ammo < gunPtr->clipSize) {
				gunPtr->ammo = 0;
				gunPtr->reloadEdit = gunPtr->reloadTime;
			}
			switch (gunPtr->upgrade) {
			default: break;
			case 'm':
				break;
			case 'r':
				break;
			case 't':
				break;
			case 'h':
				break;
			case 'l':
				break;
			case 'e':
				break;
			}
		} else {

		}
	}
	for (auto&& block : blocks) {
		int maxL = 1;
		fk::Vec2 d = block->b2Ptr->GetPosition() - b2Ptr->GetPosition();
		auto l = d.length() + 1;
		switch (block->texts.front().getText()[0]) {
		default: l *= 4; break;
		case '{':
		case '[':
		case '}':
		case ']': maxL = 0; break;
		case 'w': if (block->sprites.front().getCanvasRef().color.r == 0) { maxL = 0; break; }
		case '-':
		case '+':
		case 'x': maxL = 2; l *= 0.999 * l / 2; break;
		case '(': l *= 1; maxL = 3; break;
		case ')': l *= -1; maxL = 3; break;
		}
		if (l && d.length() < maxL) { move -= d.normalized() / l; }
	}
	if (reactors.size()) {
		if (shieldTimer == 0) {
			shieldTimer = rangen.getInt(0, 10);
			if (shieldTimer > 1) { shieldTimer = 0; }
			if (shieldTimer) { shieldTimer = rangen.getInt(0, 60); }
		} else if (shieldTimer == 1) {
			shieldTimer = shieldBreakTime + rangen.getInt(0, 120);
		}
	}
	if (shieldTimer) {
		if (shealth) {
			if (shield) { shield += 1; }
			else if (reflect > recharge) { shield += 1; shealth = 10; }
			else if (rangen.getInt(0, 60) == 0) { dodge = dodgeFrames; }
		} else {
			if (reflect > recharge * 2) { shield += 1; shealth = 10; }
			else { shield = 0; stunned = 1; immobilized = 1; }
		}
	}
	else { shield = 0; }
	if (damage > 10) { health = 0; }
	if (shieldTimer > 0) { --shieldTimer; }
	if (shieldTimer < 0) { ++shieldTimer; }
}

void Player::getPlayerInput(fk::UserInput & ui) {
	move.x = ui.getAxiInfo(joys.xMove, team);
	move.y = -ui.getAxiInfo(joys.yMove, team);
	if (move.length() < 10000) { move.x = 0; move.y = 0; }
	aim.x = ui.getAxiInfo(joys.xFire, team);
	aim.y = -ui.getAxiInfo(joys.yFire, team);
	//if (aim.length() < 10000) { aim.x = 0; aim.y = 0; }
	if (ui.getKeyInfo(fk::Key::PAD8).downFrames && team == 2) { move.y += 1; }
	if (ui.getKeyInfo(fk::Key::PAD4).downFrames && team == 2) { move.x -= 1; }
	if (ui.getKeyInfo(fk::Key::PAD6).downFrames && team == 2) { move.x += 1; }
	if (ui.getKeyInfo(fk::Key::PAD2).downFrames && team == 2) { move.y -= 1; }
	interact = ui.getJoyInfo(joys.interact, team).downFrames == 1;
	if (ui.getJoyInfo(joys.reloadEdit, team).downFrames == 1) {
		gunPtr->ammo = 0;
		gunPtr->reloadEdit = gunPtr->reloadTime;
	}
	if (
		ui.getJoyInfo(joys.dodge, team).downFrames == 1
		&& dodge == 0 && (move.x || move.y) && !immobilized
	) {
		dodge = dodgeFrames;
	}
	gunPtr->trigger = ui.getAxiInfo(joys.fire, team) != (int)fk::Joy::MINXI;
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
	if (damage > 1000) { health = 0; }
}
void Player::draw() {
	mouse.draw();
	mouse.setColor(0, 0, 0, 255);
	if (gunPtr->charge > 60) { mouse.setColor(255, 255, 255, 255); }
	auto position = b2Ptr->GetPosition();
	if (gunPtr) {
		gunPtr->text[0].setPosition(position.x, position.y);
		gunPtr->text[0].setRotationAxis(position.x, position.y);
		gunPtr->text[0].canvas.rotationAngle += fk::TAU / 2;
	}
	sprites.front().setPosition(position.x, position.y);
	sprites.back().setPosition(position.x, position.y);
	if (health < 1) {
		sprites.front().setColor(0, 0, 0, 100);
		sprites.back().setColor(0, 0, 0, 0);
		mouse.setColor(0, 0, 0, 0);
		gunPtr->text[0].setColor(0, 0, 0, 0);
	} else {
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
		if (ai) {
			if (sprites.front().getCanvasRef().color.r > 100) {
				sprites.front().getCanvasRef().color.r -= 50;
			} else { sprites.front().getCanvasRef().color.r = 64; }
			if (sprites.front().getCanvasRef().color.g > 100) {
				sprites.front().getCanvasRef().color.g -= 50;
			} else { sprites.front().getCanvasRef().color.g = 64; }
			if (sprites.front().getCanvasRef().color.b > 50) {
				sprites.front().getCanvasRef().color.b -= 50;
			} else { sprites.front().getCanvasRef().color.b = 0; }
		} else {
			if (sprites.front().getCanvasRef().color.r > 50) {
				sprites.front().getCanvasRef().color.r -= 50;
			} else { sprites.front().getCanvasRef().color.r = 0; }
			if (sprites.front().getCanvasRef().color.g > 50) {
				sprites.front().getCanvasRef().color.g -= 50;
			} else { sprites.front().getCanvasRef().color.g = 0; }
			if (sprites.front().getCanvasRef().color.b > 50) {
				sprites.front().getCanvasRef().color.b -= 50;
			} else { sprites.front().getCanvasRef().color.b = 0; }
		}
	}
	prevHealth = health;
}

void Player::setTeam(int t) {
	team = t;
	if (gunPtr) { gunPtr->team = team; }
}

void Player::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	if (myFixturePtr->IsSensor()) {
		Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
		switch (bod->type) {
		default: if (bod->team != team) { reactors.push_back(bod); } break;
		case 0: blocks.push_back(static_cast<TextBlock*>(bod)); break;
		case 1: players.push_back(static_cast<Player*>(bod)); break;
		case -1: break;
		}
	}
}

void Player::p_endCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	if (myFixturePtr->IsSensor()) {
		Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
		switch (bod->type) {
		default: if (bod->team != team) { reactors.remove(bod); } break;
		case 0: blocks.remove(static_cast<TextBlock*>(bod)); break;
		case 1: players.remove(static_cast<Player*>(bod)); break;
		case -1: break;
		}
	}
}