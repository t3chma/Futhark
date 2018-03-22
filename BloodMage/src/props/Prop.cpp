#include "Prop.h"

Prop::Prop(fk::SpriteBatch* sbPtr, fk::World& world, PropDef& pd)
	: Object(sbPtr, world, pd.bodyType, pd.position.x, pd.position.y) {
	p_spriteIDs.push_back(sbPtr->makeSprite(pd.texture));
	(*sbPtr)[p_spriteIDs.back()].setDimensions(pd.size + 0.01, pd.size + 0.01);
	(*sbPtr)[p_spriteIDs.back()].setPosition(pd.position);
	b2BodyPtr->SetLinearDamping(10);
	b2BodyPtr->SetAngularDamping(5);
	b2FixtureDef fixtureDef1;
	b2PolygonShape shape1;
	fixtureDef1.shape = &shape1;
	shape1.SetAsBox(pd.size / 2, pd.size / 2);
	fixtureDef1.userData = nullptr;
	fixtureDef1.density = 1.0f;
	fixtureDef1.friction = 0.3f;
	b2BodyPtr->CreateFixture(&fixtureDef1);
}
Prop::~Prop() {

}