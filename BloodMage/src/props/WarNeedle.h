#pragma once
#include "Prop.h"


class WarNeedle : public Prop {
  public:
	WarNeedle(Map& map, PropDef& pd, glm::vec2& throwVector, Object* linkPtr);
	~WarNeedle();
	virtual void p_postCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2ContactImpulse* impulsePtr
	) override;
	virtual void update(fk::Camera* camPtr = nullptr) override;
	Object* weldPtr{ nullptr };
	bool welded{ false };
	float size{ 0 };
	int stick{ 80 };
	glm::vec2 throwVector;
	std::vector<glm::vec2> links;
};