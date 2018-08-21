#pragma once
#include <set>
#include "Actor.h"
#include "in/UserInput.h"
#include "out/Camera.h"
#include "../Mouse.h"

struct PlayerDef {
	ActorDef ad;
	fk::Texture sword;
	fk::Texture swipe;
	fk::Texture thrown;
};

class Player : public Actor {
  public:
	struct {
		int fire{ 0 };
		int water{ 0 };
		int earth{ 0 };
		int air{ 0 };
	} drainTimer;
	Player(Map& map, fk::UserInput* uiPtr, PlayerDef& pd);
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
		M_Control(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
		virtual void updateSprite() override;
	};
	struct M_StunDash : Actor::State {
		int iFrames{ 16 };
		M_StunDash(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
	};
	struct M_Dodge : Actor::State {
		int iFrames{ 16 };
		M_Dodge(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
	};
	struct M_Enchant : Actor::State {
		std::string sequence{ "" };
		M_Enchant(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	// TODO: M_Spin
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos;
	int m_dodgeTimer{ 0 };
	struct {
		bool selfClick{ false };
		int oldCharge{ 0 };
		int const chargeTime{ 20 };
		bool charged{ false };
		int swipeTimer{ 0 };
		int const swipeRest{ 0 };
		int swipes{ 0 };
		int const maxSwipes{ 3 };
		bool thrown{ false };
		glm::vec2 target{ 0 };
		glm::vec2 position{ 0 };
		bool swipe{ false };
	} m_weapons[2];
	struct {
		int fire{ 0 };
		int water{ 0 };
		int earth{ 0 };
		int air{ 0 };
	} m_oldFlooredBlood;
	std::set<Object*> m_aoePtrs;
	std::set<Object*> m_swipeSPtrs;
	std::set<Object*> m_swipeFPtrs;
};