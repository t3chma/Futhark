#pragma once
#include "Prop.h"
class Box : public Prop {
public:
	Box(Map& map, PropDef& pd);
	~Box();
};

