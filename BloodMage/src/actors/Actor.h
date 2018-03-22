#pragma once
#include "../Object.h"


struct ActorDef {
	float speed{ 1 };
	float size{ 1 };
	glm::vec2 position{ 0,0 };
	std::vector<fk::Texture> textures;
};


class Actor : public Object {
  public:
	Actor() = delete;
	Actor(fk::SpriteBatch* sbPtr, fk::World& world, ActorDef& ad);
	~Actor();
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) = 0;
  protected:
	float p_speed{ 1 };
	glm::vec2 p_moveDirection{ 0,1 };
	glm::vec2 p_faceDirection{ 0,1 };
	float p_faceAngle{ 0 };
};