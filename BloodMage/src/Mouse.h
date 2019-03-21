#pragma once
#include "Object.h"
#include "Map.h"
#include "Orders.h"
#include "actors/Actor.h"


class Mouse : public Object {
  public:
	Actor* acotrEditPtr{ nullptr };
	Order* orderEditPtr{ nullptr };
	Mouse(Map& map, fk::Texture& texture);
	~Mouse() = default;
	void updateBody() override;
	void updateSprite() override;
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
	Object* objectPtr;
	glm::vec2 position{ 0 };
};
