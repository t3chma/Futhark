#pragma once
#include "Actor.h"


class Grunt : public Actor {
public:
	Grunt(Map& map, ActorDef& ad);
	~Grunt();
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
	struct Agro : Actor::State {
		fk::Random rangen;
		int direction{ 1 };
		float range{ 8 };
		bool justEntered{ true };
		virtual void enter() override;
		Agro(Actor& actor) : State(actor) { if (rangen.getInt(0,1)) { direction *= -1; } };
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	struct Charge: Actor::State {
		bool justEntered{ true };
		Charge(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	struct Attack : Actor::State {
		int timer{ 1 };
		Attack(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
  private:
	std::list<Object*> m_hitPtrs;
};