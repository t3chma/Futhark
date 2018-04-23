#pragma once
#include "Actor.h"
#include "in/UserInput.h"
#include "out/Camera.h"
#include "../Mouse.h"

class Player : public Actor {
  public:
	struct {
		int fire{ 0 };
		int water{ 0 };
		int earth{ 0 };
		int air{ 0 };
	} drainTimer;
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
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
  private:
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos;
	int m_leftSwipe{ 0 };
	int m_rightSwipe{ 0 };
	bool m_getTarget{ false };
	int m_dodgeCharge{ 0 };
	int m_dodgeTimer{ 0 };
	glm::vec2 m_dodgePos{ 0 };
	const int m_dodgeIFrames{ 15 };
	bool m_charging{ false };
	struct {
		int fire{ 0 };
		int water{ 0 };
		int earth{ 0 };
		int air{ 0 };
	} oldFloor;
};