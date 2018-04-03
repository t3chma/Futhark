#pragma once
#include "../Object.h"
#include "../Map.h"

struct PropDef {
	float size{ 1 };
	glm::vec2 position{ 0,0 };
	fk::Texture texture{ 0 };
	bool physics;
};


class Prop : public Object {
public:
	Prop() = delete;
	Prop(Map& map, PropDef& pd);
	~Prop();
	virtual void update(fk::Camera* camPtr = nullptr) {};
protected:
};