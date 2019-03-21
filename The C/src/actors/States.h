#pragma once
#include <vector>
#include "out/Camera.h"
class Actor;

struct State {
	Actor* actorPtr{ nullptr };
	State() = delete;
	State(Actor& actor) : actorPtr(&actor) {};
	virtual void enter() {};
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) {};
	virtual void updateBody();
	virtual void updateSprite() {};
};
struct AgroState : public State {
	AgroState() = delete;
	AgroState(Actor& actor) : State(actor) {};
	virtual State* copy() = 0;
};
struct Dead : public State {
	Dead(Actor& actor) : State(actor) {};
	virtual void enter() override;
	virtual void updateBody() {};
};
struct Idle : State {
	Idle(Actor& actor) : State(actor) {};
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
};
struct Search : State {
	Search(Actor& actor) : State(actor) {};
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
};
struct Flee : State {
	Flee(Actor& actor) : State(actor) {};
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
};