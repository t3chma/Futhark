#pragma once
#include "../Object.h"
#include "../Map.h"

struct PropDef {
	float size{ 1 };
	glm::vec2 position{ 0, 0 };
	fk::Texture texture{ 0 };
	bool physics{ true };
	float angularDamping{ 32.0f };
	float linearDamping{ 8.0f };
	float friction{ 0.3f };
	float density{ 5.0f };
	bool bullet{ false };
};


class Prop : public Object {
public:
	Prop() = delete;
	Prop(Map& map, PropDef& pd);
	~Prop();
	virtual void update(fk::Camera* camPtr = nullptr) {};
protected:
};