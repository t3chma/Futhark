#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md), gun(sb, world, pd.md.body)
{
	setTeam(1);
	type = 1;
	addCircleLimb(0.2);
	b2Ptr->SetLinearDamping(50);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(500);
	limbs.back().b2Ptr->SetRestitution(0);
	p_sprites.emplace_back(p_spriteBatch, pd.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(0.4, 0.4);
}
Player::~Player() {

}
void Player::update(fk::UserInput& ui) {
	mouse.update(ui);
	if (health > 0) {
		auto direction = glm::vec2(0);
		if (ui.getKeyInfo(right).downFrames > 1) { direction.x += 1; }
		if (ui.getKeyInfo(left).downFrames > 1) { direction.x -= 1; }
		if (ui.getKeyInfo(up).downFrames > 1) { direction.y += 1; }
		if (ui.getKeyInfo(down).downFrames > 1) { direction.y -= 1; }
		auto normalDir = (direction.x || direction.y) ? glm::normalize(direction) : direction;
		float speed = 100;
		b2Vec2 myPos = b2Ptr->GetPosition();
		glm::vec2 myPosition = glm::vec2(myPos.x, myPos.y);
		b2Ptr->ApplyForceToCenter(b2Vec2(normalDir.x * speed, normalDir.y * speed), true);
		if (ui.getKeyInfo(fire).downFrames == 1) {
			gun.fire(b2Ptr->GetWorldCenter(), mouse.b2Ptr->GetWorldCenter());
		}
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