#pragma once
#include "out/SpriteBatch.h"
#include "Box2D/Box2D.h"
#include "out/Camera.h"
#include "up/Body.h"
#include "up/World.h"


class Object : public fk::Body {
public:
	Object() = delete;
	Object(
		fk::SpriteBatch* sbPtr,
		fk::World& world,
		b2BodyType type,
		float xPosition = 0.0f,
		float yPosition = 0.0f,
		float angle = 0.0f,
		bool fixedRotation = false,
		bool bullet = false
	);
	glm::vec2 getPosition();
	virtual void updateBody() {};
	virtual void updateSprite() {};
protected:
	fk::SpriteBatch* p_sbPtr{ nullptr };
	std::vector<int> p_spriteIDs;
	int p_health{ 100 };
};