#pragma once
#include "up/Body.h"
#include "out/SpriteBatch.h"
#include "in/UserInput.h"

class Actor : public fk::Body {
  public:
	Actor(fk::SpriteBatch& sb, fk::World& world);
	virtual void update(fk::UserInput& ui) = 0;
	virtual void draw() = 0;
  protected:
	long p_health{ 0 };
	int p_speed{ 0 };
	glm::vec2 p_position{ 0 };
	glm::vec2 p_direction{ 0 };
	fk::SpriteBatch& p_spriteBatch;
};

class Player : public Actor {
  public:
	void update(fk::UserInput& ui) {
		p_direction = glm::vec2(0);
		if (ui.getKeyInfo(fk::Key::D).downFrames > 1) { p_direction.x += 1; }
		if (ui.getKeyInfo(fk::Key::A).downFrames > 1) { p_direction.x -= 1; }
		if (ui.getKeyInfo(fk::Key::W).downFrames > 1) { p_direction.y += 1; }
		if (ui.getKeyInfo(fk::Key::S).downFrames > 1) { p_direction.y -= 1; }
		float speed = p_speed;
		if (ui.getKeyInfo(fk::Key::SHIFT_L).downFrames > 1) { speed = p_speed * 2; }
		auto dir = glm::normalize(p_direction);
		p_position.x += dir.x * speed;
		p_position.y += dir.y * speed;
	};
};