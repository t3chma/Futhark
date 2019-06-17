#include "Object.h"

Object::Object(
	fk::World& world,
	b2BodyType type,
	float xPosition,
	float yPosition,
	float linearDamping,
	float angularDamping,
	float angle,
	bool fixedRotation,
	bool bullet
) : Body(
		world,
		type,
		xPosition,
		yPosition,
		angle,
		fixedRotation,
		bullet,
		linearDamping,
		angularDamping
	) {
}
glm::vec2 Object::getPosition() {
	b2Vec2 vec = b2Ptr->GetPosition();
	return glm::vec2(vec.x, vec.y);
}
void Object::updateSprites() {
	for (auto&& sprite : sprites) {
		sprite.setRotationAngle(b2Ptr->GetAngle());
		sprite.setPosition(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
		sprite.setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	}
}

inline void Object::addSprite(fk::SpriteBatch& spriteBatch, const fk::Texture& texture) {
	sprites.emplace_back(spriteBatch, texture);
}

