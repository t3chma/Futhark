#pragma once
#include <set>
#include "Actor.h"

struct GruntDef {
	ActorDef ad;
	fk::Texture swipe;
};

class Grunt : public Actor {
public:
	Grunt(Map& map, GruntDef& gd);
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
	struct Agro : Actor::AgroState {
		fk::Random rangen;
		int direction{ 1 };
		float range{ 8 };
		bool justEntered{ true };
		virtual void enter() override;
		Agro(Actor& actor) : AgroState(actor) { if (rangen.getInt(0,1)) { direction *= -1; } };
		State* copy() override { return new Grunt::Agro(*this->actorPtr); };
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
	std::set<Object*> m_swipePtrs;
};