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
		fk::World& world,
		b2BodyType type,
		float xPosition = 0.0f,
		float yPosition = 0.0f,
		float angularDamping = 10,
		float linearDamping = 10,
		float angle = 0.0f,
		bool fixedRotation = false,
		bool bullet = false
	);
	virtual glm::vec2 getPosition();
	virtual void updateBody() {};
	virtual void updateSprites();
	inline virtual void addSprite(fk::SpriteBatch& spriteBatch, const fk::Texture& texture);
	std::list<fk::Sprite> sprites;
	int health{ 100 };
	std::string type{ "" };
};