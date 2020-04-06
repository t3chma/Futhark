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
	int damage{ 0 };
	bool toggleCam{ false };
	glm::vec2 gravMod{ 0 };
	Body* ownerPtr{ nullptr };
	double torque{ 0 };
	double gorque{ 0 };
	int charge{ 0 };
	float resistance{ 10 };
	int level{ 0 };
	int timer{ 0 };
	int timers{ 0 };
};