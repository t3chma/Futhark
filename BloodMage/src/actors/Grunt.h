#pragma once
#include "up/Actor.h"

class Grunt : public fk::Actor {
public:
	enum State { RESTING, CASTING, CIRCLING, CHARGING, RETREATING, DEAD };
	Grunt(fk::SpriteBatch* sbPtr, fk::World& world, fk::ActorDef& ad);
	~Grunt();
	void think(std::vector<fk::Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
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
	int m_range{ 10 };
	std::vector<fk::Body*> m_leftHitPtrs;
	std::vector<fk::Body*> m_rightHitPtrs;
	fk::Random m_rangen;
	int m_counter{ 0 };
	bool m_canAttack{ false };
	bool m_attackingLeft{ false };
	bool m_attackingRight{ false };
	State m_state{ RESTING };
};