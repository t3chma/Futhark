#pragma once
#include <set>
#include "Actor.h"
#include "in/UserInput.h"
#include "out/Camera.h"
#include "../Mouse.h"
#include "States.h"
#define _TRAIL_ 100

struct PlayerDef {
	ActorDef ad;
};

class Player : public Actor {
  public:
	struct {
		fk::Sprite* body{ nullptr };
	} spritePtrs;
	Player(fk::World& world, fk::UserInput* uiPtr, PlayerDef& pd);
	~Player();
	void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	void updateSprites() override;
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
	float32 ReportFixture(
		b2Fixture* fixture,
		const b2Vec2& point,
		const b2Vec2& normal,
		float32 fraction
	);
  private:
	struct M_Control : State {
		M_Control(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
		virtual void updateSprite() override;
	};
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos;
	glm::vec2 m_oldPos[_TRAIL_];
	float m_oldAng[_TRAIL_];
	fk::Vec2 m_wakeReset;
};