#pragma once
#include "up/Actor.h"
#include "in/UserInput.h"
#include "out/Camera.h"

class Player : public fk::Actor {
  public:
	Player(fk::SpriteBatch* sbPtr, b2World* wPtr, fk::UserInput* uiPtr, fk::ActorDef& ad);
	~Player();
	void think(std::vector<fk::Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	void updateBody() override;
	void updateSprite() override;
  private:
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos{ 0 };
	bool m_leftSwipe{ false };
	bool m_rightSwipe{ false };
};