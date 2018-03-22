#include "Object.h"


Object::Object(
	fk::SpriteBatch* sbPtr,
	fk::World& world,
	b2BodyType type,
	float xPosition,
	float yPosition,
	float angle,
	bool fixedRotation,
	bool bullet
) : p_sbPtr(sbPtr), Body(world, type, xPosition, yPosition, angle, fixedRotation, bullet) {

}
glm::vec2 Object::getPosition() {
	b2Vec2 vec = b2BodyPtr->GetPosition();
	return glm::vec2(vec.x, vec.y);
}

