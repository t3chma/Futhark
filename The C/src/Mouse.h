#pragma once
#include "Object.h"
#include "actors/Actor.h"


class Mouse : public Object {
  public:
	Mouse(fk::SpriteBatch& sb, fk::Texture& texture, fk::World& world);
	~Mouse() = default;
	void updateBody() override;
	void updateSprites() override;
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
