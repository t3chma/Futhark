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
	Body(fk::World& world, Body::Def botSpawner = Body::Def())
		: fk::Body(
			world, b2_dynamicBody, botSpawner.position.x, botSpawner.position.y, botSpawner.angle
		)
	{};
	~Body() {};
	virtual bool connect(Boid* bPtr) { return true; };
	virtual bool interact(Boid* bPtr) { return true; };
  protected:
	long p_health{ 100000 };
};