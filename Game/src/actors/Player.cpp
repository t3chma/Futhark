#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md), gun(sb, world, pd.md.body)
{
	setTeam(1);
	type = 1;
	addCircleLimb(0.2);
	b2Ptr->SetLinearDamping(10);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(200);
	limbs.back().b2Ptr->SetRestitution(0);
	p_sprites.emplace_back(p_spriteBatch, pd.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(0.4, 0.4);
	p_sprites.back().getCanvasRef().position.z = 10;
}
Player::~Player() {

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
		bool nt;
		if (ui.getAxiInfo(joys.fire, team) != (int)fk::Joy::MINXI) { nt = true; }
		else { nt = false; };
		if (aim.x || aim.y) {
			mouse.setColor(255, 255, charge > 60 ? 0 : 255, 255);
			mouse.b2Ptr->SetTransform(b2Vec2(myPosition.x + aim.x / 4, myPosition.y + aim.y / 4), 0);
			if (nt != t) {
				if (!nt) {
					aim.x *= 1000;
					aim.y *= 1000;
					mouse.setColor(255, 0, 0, 255);
					gun.fire(this, b2Ptr->GetWorldCenter(), aim);
					charge = 0;
					crank = false;
				} else {
					crank = true;
				}
			}
		}
		if (crank) { ++charge; }
		t = nt;
	}
}
void Player::draw() {
	mouse.draw();
	gun.draw();
	auto position = b2Ptr->GetPosition();
	p_sprites.front().setPosition(position.x, position.y);
	if (prevHealth && health < 1) { p_sprites.front().setColor(0,0,0,255); }
	prevHealth = health;
}