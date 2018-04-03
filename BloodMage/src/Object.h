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
		fk::SpriteBatch& spriteBatch,
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
	virtual void updateSprite();
	fk::SpriteBatch& spriteBatch;
	std::vector<int> spriteIDs;
	int health{ 100 };
	bool hit{ false };
	bool despawn{ false };
	std::string type{ "" };
};