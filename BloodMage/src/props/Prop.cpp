#include "Prop.h"

Prop::Prop(Map& map, PropDef& pd)
	: Object(
		map,
		pd.physics ? b2_dynamicBody : b2_kinematicBody,
		pd.position.x,
		pd.position.y,
		pd.angularDamping,
		pd.linearDamping,
		0,
		false,
		pd.bullet
	) {
	// Updating
	map.propPtrs.push_back(this);
	// Graphics
	sprites.emplace_back(map.dynamicObjectSprites, pd.texture);
	sprites.back().setDimensions(pd.size + 0.01, pd.size + 0.01);
	sprites.back().setPosition(pd.position.x, pd.position.y);
}
Prop::~Prop() {
	for (auto&& propPtr : map.propPtrs) {
		if (propPtr == this) {
			propPtr = map.propPtrs.back();
			map.propPtrs.pop_back();
		}
	};
}