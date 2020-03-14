#pragma once
#include "up/Body.h"
#include "Constants.h"
#include "out/OutUtility.h"

class Body : public fk::Body {
  public:
	struct Def {
		fk::Vec2 position{ 0, 0 };
		float angle{ 0 };
	};
	Body(fk::World& world, Body::Def bs = Body::Def())
		: fk::Body(
			world, b2_dynamicBody, bs.position.x, bs.position.y, bs.angle
		)
	{};
	~Body() {};
	long health{ 1 };
};