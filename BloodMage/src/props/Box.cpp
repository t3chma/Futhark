#include "Box.h"


Box::Box(Map& map, PropDef& pd) : Prop(map, pd) {
	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.density = pd.density;
	fixtureDef.friction = pd.friction;
	fixtureDef.filter.categoryBits = 4;
	category = "prop";
	addRectangleLimb(pd.size / 2, pd.size / 2, 0, 0, 0, &fixtureDef);
}

Box::~Box() {

}