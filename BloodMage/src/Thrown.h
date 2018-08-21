#pragma once
#include <set>
#include "props/Prop.h"
#include "Map.h"

class Thrown : public Prop {
  public:
	Thrown(Map& map, PropDef pd, glm::vec2& direction);
	~Thrown();
	virtual void update(fk::Camera* camPtr = nullptr) override;
	virtual void updateBody() override;
	virtual void updateSprite() override;
	void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	Object* m_hitPtr{ nullptr };
};

