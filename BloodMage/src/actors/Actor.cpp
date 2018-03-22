#include "Actor.h"

Actor::Actor(fk::SpriteBatch* sbPtr, fk::World& world, ActorDef& ad)
	: Object(sbPtr, world, b2_dynamicBody, ad.position.x, ad.position.y) {
	p_spriteIDs.reserve(ad.textures.size());
	for (auto&& texture : ad.textures) {
		p_spriteIDs.push_back(sbPtr->makeSprite(texture));
		(*sbPtr)[p_spriteIDs.back()].setDimensions(ad.size, ad.size);
		(*sbPtr)[p_spriteIDs.back()].setPosition(ad.position);
	}
	b2BodyPtr->SetLinearDamping(10);
	b2BodyPtr->SetAngularDamping(5);
	b2FixtureDef fixtureDef1;
	b2CircleShape shape1;
	fixtureDef1.shape = &shape1;
	shape1.m_radius = ad.size / 2 - 0.01;
	fixtureDef1.userData = nullptr;
	fixtureDef1.density = 1.0f;
	fixtureDef1.friction = 0.3f;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	p_speed = ad.speed;
}
Actor::~Actor() {

}