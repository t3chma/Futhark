#pragma once
#include "up/Body.h"
#include "Constants.h"
#include "out/OutUtility.h"
class Boid;

/* The physical manifestation of something.

[t3chma]
*/
class Body : public fk::Body {
  public:
	struct Def {
		fk::Vec2 position{ 0, 0 };
		float angle{ 0 };
		bool antiTunneling{ false };
	};
	struct {
		Body* markPtr{ nullptr };
		Body* bondPtr{ nullptr };
		int chained{ false };
		b2Vec2 chainedLocation{ 0, 0 };
		int healing{ 0 };
		double healRate{ 0 };
		float timeDilation{ 1 };
		int blind{ false };
		float speedBoost{ 1.0 };
	} statEffects;
	Body(fk::World& world, Body::Def bd = Body::Def())
		: fk::Body(world, b2_dynamicBody, bd.position.x, bd.position.y, bd.angle, false, bd.antiTunneling)
	{};
	~Body() {};
  protected:
	double p_maxHealth{ 100 };
	double p_health{ 0 };
	double p_maxHeat{ 100 };
	double p_heat{ 0 };
	double p_couruption{ 0 };
  public:
	virtual void update() final {
		if (statEffects.timeDilation != 1) {
			auto velocity = b2Ptr->GetLinearVelocity();
			b2Ptr->SetLinearVelocity(b2Vec2(velocity.x * statEffects.timeDilation, velocity.y * statEffects.timeDilation));
			statEffects.timeDilation = 1;
		}
		if (statEffects.healing) {
			--statEffects.healing;
			if ((p_health += statEffects.healRate) > p_maxHealth - p_couruption) {
				p_health = p_maxHealth - p_couruption;
			}
		}
		if (statEffects.blind) { --statEffects.blind; }
		if (statEffects.chained) {
			--statEffects.chained;
			b2Ptr->ApplyForceToCenter(30 * (statEffects.chainedLocation - b2Ptr->GetPosition()), true);
		}
		if (p_heat > p_maxHeat) { p_health -= (double)(p_heat - p_maxHeat) / fk::SECOND; }
		if (p_heat > 0) { p_heat -= 1 / fk::SECOND; }
	};
	virtual bool connect(Boid* bPtr) { return true; };
	virtual bool interact(Boid* bPtr) { return true; };
	virtual void damage(double damage) { p_health -= damage; };
	virtual void heat(double heat) { p_heat += heat; };
};