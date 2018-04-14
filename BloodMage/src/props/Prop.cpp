#include "Prop.h"

Prop::Prop(Map& map, PropDef& pd)
	: Object(
		map.dynamicObjectSprites,
		map.world,
		pd.physics ? b2_dynamicBody : b2_kinematicBody,
		pd.position.x,
		pd.position.y
) {
	map.propPtrs.push_back(this);
	spriteIDs.push_back(spriteBatch.makeSprite(pd.texture));
	spriteBatch[spriteIDs.back()].setDimensions(pd.size + 0.01, pd.size + 0.01);
	spriteBatch[spriteIDs.back()].setPosition(pd.position);
	b2BodyPtr->SetLinearDamping(10);
	b2BodyPtr->SetAngularDamping(5);
	b2FixtureDef fixtureDef1;
	b2PolygonShape shape1;
	fixtureDef1.shape = &shape1;
	shape1.SetAsBox(pd.size / 2, pd.size / 2);
	fixtureDef1.userData = nullptr;
	fixtureDef1.density = 1.0f;
	fixtureDef1.friction = 0.3f;
	fixtureDef1.filter.categoryBits = 4;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	category = "prop";
}
Prop::~Prop() {

}