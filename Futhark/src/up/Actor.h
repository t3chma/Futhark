#pragma once
#include "../out/SpriteBatch.h"
#include "Box2D/Box2D.h"
#include "../out/Camera.h"
#include "Body.h"
#include "World.h"
namespace fk {


struct ActorDef {
	float speed{ 1 };
	float size{ 1 };
	glm::vec2 position{ 0,0 };
	std::vector<Texture> textures;
};


class Actor : public Body {
  public:
	Actor() = delete;
	Actor(SpriteBatch* sbPtr, World& world, ActorDef& ad);
	~Actor();
	virtual void think(std::vector<Actor*>& actorPtrs, Camera* camPtr = nullptr) = 0;
	virtual void updateBody() = 0;
	virtual void updateSprite() = 0;
	glm::vec2 getPosition();
  protected:
	SpriteBatch* p_sbPtr{ nullptr };
	std::vector<int> p_spriteIDs;
	float p_speed{ 1 };
	glm::vec2 p_moveDirection{ 0,1 };
	glm::vec2 p_faceDirection{ 0,1 };
	float p_faceAngle{ 0 };
	int p_health{ 100 };
};

}