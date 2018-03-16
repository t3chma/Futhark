#pragma once
#include "up/Actor.h"

class Grunt : public fk::Actor {
public:
	Grunt(fk::SpriteBatch* sbPtr, b2World* wPtr, fk::ActorDef& ad);
	~Grunt();
	void think(std::vector<fk::Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	void updateBody() override;
	void updateSprite() override;
};