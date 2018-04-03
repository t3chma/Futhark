#pragma once
#include "Actor.h"
#include "in/UserInput.h"
#include "out/Camera.h"

class Player : public Actor {
  public:
	Player(Map& map, fk::UserInput* uiPtr, ActorDef& ad);
	~Player();
	void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	void updateBody() override;
	void updateSprite() override;
  private:
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos{ 0 };
	bool m_leftSwipe{ false };
	bool m_rightSwipe{ false };
	std::list<fk::Body*> m_leftHitPtrs;
	std::list<fk::Body*> m_rightHitPtrs;
	std::list<fk::Body*> m_swipeRangePtrs;
};