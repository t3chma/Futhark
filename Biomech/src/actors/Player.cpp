#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::World& world) : Actor(sb, world) {

}
Player::~Player() {

}
void Player::update(fk::UserInput& ui) {
	p_direction = glm::vec2(0);
	if (ui.getKeyInfo(right).downFrames > 1) { p_direction.x += 1; }
	if (ui.getKeyInfo(left).downFrames > 1) { p_direction.x -= 1; }
	if (ui.getKeyInfo(up).downFrames > 1) { p_direction.y += 1; }
	if (ui.getKeyInfo(down).downFrames > 1) { p_direction.y -= 1; }
	float speed = p_speed;
	if (ui.getKeyInfo(run).downFrames > 1) { speed = p_speed * 2; }
	auto dir = glm::normalize(p_direction);
	b2Ptr->ApplyForceToCenter(b2Vec2(dir.x * speed, dir.y * speed), true);
}
void Player::draw() {

}