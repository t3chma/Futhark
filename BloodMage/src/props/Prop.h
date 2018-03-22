#pragma once
#include "../Object.h"


struct PropDef {
	float size{ 1 };
	glm::vec2 position{ 0,0 };
	fk::Texture texture{ 0 };
	b2BodyType bodyType{ b2_dynamicBody };
};


class Prop : public Object {
public:
	Prop() = delete;
	Prop(fk::SpriteBatch* sbPtr, fk::World& world, PropDef& pd);
	~Prop();
	virtual void update(fk::Camera* camPtr = nullptr) {};
protected:
};