#pragma once
#include "up/Actor.h"
#include "boost/circular_buffer.hpp"


class Spear : public fk::Actor {
public:
	enum State { RESTING, CASTING, CIRCLING, CHARGING, RETREATING, DEAD, STABBING };
	Spear(fk::SpriteBatch* sbPtr, fk::World& world, fk::ActorDef& ad);
	~Spear();
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
	std::vector<fk::Body*> m_hitPtrs;
	std::vector<fk::Body*> m_rangePtrs;
	fk::Random m_rangen;
	int m_counter{ 0 };
	bool m_canAttack{ false };
	bool m_attacking{ false };
	bool m_stabbing{ false };
	float m_targetAngle{ 0 };
	State m_state{ RESTING };
};