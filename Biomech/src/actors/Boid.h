#pragma once
#include "Actor.h"
#include <set>


class Boid : public Actor {
  public:
	struct Los {
		Los(float radius);
		float radius{ 0 };
		std::vector<float> friendAngles;
		std::vector<glm::vec2> friendPos;
		std::vector<glm::vec2> badPos;
	} los;
	glm::vec2 goal{0};
	Boid(fk::SpriteBatch& sb, fk::World& world, float sightRadius);
	~Boid();
	virtual void update(fk::UserInput& ui) override;
	void draw() override;
  protected:
	inline virtual void p_postCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2ContactImpulse* impulsePtr
	) override;
};

