#include "Object.h"


Object::Object(
	fk::SpriteBatch& spriteBatch,
	fk::World& world,
	b2BodyType type,
	float xPosition,
	float yPosition,
	float angle,
	bool fixedRotation,
	bool bullet
) : sprites(spriteBatch), Body(world, type, xPosition, yPosition, angle, fixedRotation, bullet) {

}
glm::vec2 Object::getPosition() {
	b2Vec2 vec = b2BodyPtr->GetPosition();
	return glm::vec2(vec.x, vec.y);
}
void Object::updateSprite() {
	for (auto&& spriteID : sprites.ids) {
		sprites.batch[spriteID.second].canvas.rotationAngle = b2BodyPtr->GetAngle();
		sprites.batch[spriteID.second].setPosition(
			b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
		);
		sprites.batch[spriteID.second].setRotationAxis(
			b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y
		);
	}
}

