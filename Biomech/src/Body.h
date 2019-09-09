#pragma once
#include "up/Body.h"
#include "Constants.h"
#include "out/OutUtility.h"
class Boid;


class Body : public fk::Body {
  public:
	struct Def {
		fk::Vec2 position{ 0, 0 };
		float angle{ 0 };
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
		: fk::Body(world, b2_dynamicBody, bd.position.x, bd.position.y, bd.angle)
	{};
	~Body() {};
  protected:
	double p_maxHealth{ 100 };
	double p_health{ 50 };
  public:
	virtual void update() final {
		if (statEffects.timeDilation != 1) {
			auto velocity = b2Ptr->GetLinearVelocity();
			b2Ptr->SetLinearVelocity(b2Vec2(velocity.x * statEffects.timeDilation, velocity.y * statEffects.timeDilation));
		}
		if (statEffects.healing) {
			--statEffects.healing;
			if ((p_health += statEffects.healRate) > p_maxHealth) {
				p_health = p_maxHealth;
			}
		}
		if (statEffects.blind) { --statEffects.blind; }
		if (statEffects.chained) {
			--statEffects.chained;
			b2Ptr->ApplyForceToCenter(30 * (statEffects.chainedLocation - b2Ptr->GetPosition()), true);
		}
	};
	virtual bool connect(Boid* bPtr) { return true; };
	virtual bool interact(Boid* bPtr) { return true; };
	virtual void damage(double damage) { p_health -= damage; };
};