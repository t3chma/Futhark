#include "Actor.h"
namespace fk {


Actor::Actor(SpriteBatch* sbPtr, b2World* wPtr, ActorDef& ad) : p_sbPtr(sbPtr) {
	p_spriteIDs.reserve(ad.textures.size());
	for (auto&& texture : ad.textures) {
		p_spriteIDs.push_back(sbPtr->makeSprite(texture));
		(*sbPtr)[p_spriteIDs.back()].setDimensions(ad.size, ad.size);
		(*sbPtr)[p_spriteIDs.back()].setPosition(ad.position);
		(*sbPtr)[p_spriteIDs.back()].setDepth(0);
	}
	b2BodyDef bodyDef1;
	bodyDef1.linearDamping = 10;
	bodyDef1.angularDamping = 5;
	bodyDef1.type = b2_dynamicBody;
	bodyDef1.position.Set(ad.position.x, ad.position.y);
	p_bodyPtr = wPtr->CreateBody(&bodyDef1);
	b2CircleShape shape1;
	shape1.m_radius = ad.size / 2;
	b2FixtureDef fixtureDef1;
	fixtureDef1.shape = &shape1;
	fixtureDef1.density = 1.0f;
	fixtureDef1.friction = 0.3f;
	p_bodyPtr->CreateFixture(&fixtureDef1);
	p_speed = ad.speed;
}


Actor::~Actor() {

}

glm::vec2 Actor::getPosition() {
	b2Vec2 vec = p_bodyPtr->GetPosition();
	return glm::vec2(vec.x, vec.y);
}

}