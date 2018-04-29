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
	void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	void updateSprite() override;
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
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
  private:
	struct M_Control : Actor::State {
		fk::Random rangen;
		float range{ 4 };
		M_Control(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	struct M_StunDash : Actor::State {
		int iFrames{ 15 };
		M_StunDash(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
	};
	struct M_Attack : Actor::State {
		int drainCount{ 32 };
		std::array<int, 2> mouseButtons{0,0};
		bool dodgeButton{ 0 };
		M_Attack(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
		virtual void updateSprite() override;
	};
	struct M_Dodge : Actor::State {
		int iFrames{ 16 };
		M_Dodge(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
	};
	struct M_Cast : Actor::State {
		M_Cast(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos;
	int m_leftSwipeTimer{ 0 };
	int m_rightSwipeTimer{ 0 };
	int m_leftSwipeRest{ 20 };
	int m_rightSwipeRest{ 20 };
	int m_leftSwipes{ 0 };
	int m_rightSwipes{ 0 };
	int m_leftMaxSwipes{ 3 };
	int m_rightMaxSwipes{ 3 };
	int m_dodgeTimer{ 0 };
	bool m_readyToDash{ false };
	struct {
		int fire{ 0 };
		int water{ 0 };
		int earth{ 0 };
		int air{ 0 };
	} m_oldFloor;
};