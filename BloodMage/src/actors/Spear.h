#pragma once
#include "Actor.h"
#include "boost/circular_buffer.hpp"


class Spear : public Actor {
public:
	enum State { RESTING, CASTING, CIRCLING, CHARGING, RETREATING, DEAD, STABBING };
	Spear(Map& map, ActorDef& ad);
	~Spear();
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
protected:
	bool m_swipeRange{ false };
	bool m_direction{ true };
	int m_range{ 2 };
	std::list<Object*> m_hitPtrs;
	std::list<Object*> m_rangePtrs;
	fk::Random m_rangen;
	int m_counter{ 0 };
	bool m_canAttack{ false };
	bool m_attacking{ false };
	bool m_stabbing{ false };
	float m_targetAngle{ 0 };
	State m_state{ RESTING };
};