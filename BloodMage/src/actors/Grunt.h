#pragma once
#include "Actor.h"

class Grunt : public Actor {
public:
	enum State { RESTING, CASTING, CIRCLING, CHARGING, RETREATING, DEAD };
	Grunt(fk::SpriteBatch* sbPtr, fk::World& world, ActorDef& ad);
	~Grunt();
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
	bool m_direction{ true };
	float m_range{ 10 };
	std::list<fk::Body*> m_leftHitPtrs;
	std::list<fk::Body*> m_rightHitPtrs;
	std::list<fk::Body*> m_swipeRangePtrs;
	fk::Random m_rangen;
	int m_counter{ 0 };
	bool m_canAttack{ false };
	bool m_attackingLeft{ false };
	bool m_attackingRight{ false };
	State m_state{ RESTING };
	int test{ 0 };
};