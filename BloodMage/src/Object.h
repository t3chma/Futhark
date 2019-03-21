#pragma once
#include "out/SpriteBatch.h"
#include "Box2D/Box2D.h"
#include "out/Camera.h"
#include "up/Body.h"
#include "up/World.h"
class Map;

class Object : public fk::Body {
public:
	Object() = delete;
	Object(
		Map& map,
		b2BodyType type,
		float xPosition = 0.0f,
		float yPosition = 0.0f,
		float angularDamping = 10,
		float linearDamping = 10,
		float angle = 0.0f,
		bool fixedRotation = false,
		bool bullet = false
	);
	Map& map;
	virtual glm::vec2 getPosition();
	virtual void updateBody() {};
	virtual void updateSprites();
	std::list<fk::Sprite> sprites;
	int health{ 100 };
	bool hit{ false };
	bool despawn{ false };
	std::string type{ "" };
};