#include "Player.h"


Player::Player(fk::SpriteBatch& sb, fk::SpriteBatch& textBatch, fk::World& world, Player::Def pd)
	: Body(world, pd.bd), Image(sb), mouse(sb, world, pd.md)
{
	addCircleLimb(0.2);
	b2Ptr->SetLinearDamping(10);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(10);
	limbs.back().b2Ptr->SetRestitution(0);
	p_sprites.emplace_back(p_spriteBatch, pd.body);
	p_sprites.back().setColor(255, 255, 255, 255); // white
	p_sprites.back().setDimensions(0.4, 0.4);
}
Player::~Player() {

}
void Player::update(fk::UserInput& ui) {
	mouse.update(ui);
	auto direction = glm::vec2(0);
	if (ui.getKeyInfo(right).downFrames > 1) { direction.x += 1; }
	if (ui.getKeyInfo(left).downFrames > 1) { direction.x -= 1; }
	if (ui.getKeyInfo(up).downFrames > 1) { direction.y += 1; }
	if (ui.getKeyInfo(down).downFrames > 1) { direction.y -= 1; }
	auto normalDir = (direction.x || direction.y) ? glm::normalize(direction) : direction;
	float speed = 15;
	b2Vec2 myPos = b2Ptr->GetPosition();
	glm::vec2 myPosition = glm::vec2(myPos.x, myPos.y);
if (ui.getKeyInfo(fk::Key::CTRL_L).downFrames || ui.getKeyInfo(fk::Key::CTRL_R).downFrames) {
		if (ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1) {

		}
		if (ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1) {

		}
		if (ui.getKeyInfo(fk::Key::MOUSE_MIDDLE).downFrames == 1) {

		}
	} else {
		int priority{ -1 };
		static int mouseLeft{ 0 };
		int left{ ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames };
		if (mouseLeft && left == 0) {
			mouse.click(priority = 0);
			b2Vec2 mousePos = mouse.b2Ptr->GetPosition();
			glm::vec2 mousePosition = glm::vec2(mousePos.x, mousePos.y);
		}
		mouseLeft = ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames;
		static int mouseRight{ 0 };
		int right{ ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames };
		if (mouseRight && right == 0) {
			mouse.click(priority = 1);
			b2Vec2 mousePos = mouse.b2Ptr->GetPosition();
			glm::vec2 mousePosition = glm::vec2(mousePos.x, mousePos.y);
		}
		mouseRight = ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames;
	}
	b2Ptr->ApplyForceToCenter(b2Vec2(normalDir.x * speed, normalDir.y * speed), true);
}
void Player::draw() {
	mouse.draw();
	auto position = b2Ptr->GetPosition();
	p_sprites.front().setPosition(position.x, position.y);
}