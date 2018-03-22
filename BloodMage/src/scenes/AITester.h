#pragma once
#include "base/Scene.h"
#include "out/SpriteRenderer.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "../actors/Spear.h"
#include "../actors/Grunt.h"
#include "../actors/Player.h"
#include "../props/Prop.h"

class AITester : public fk::Scene {
  public:
	std::vector<fk::SpriteBatch> sprites;
	fk::SpriteRenderer spriteRenderer;
	std::vector<int> spriteIDs;
	fk::Camera cam;
	glm::vec2 camMovement{ 0 };
	fk::World world;
	std::vector<Actor*> actorPtrs;
	std::vector<Prop*> propPtrs;
	std::vector<b2Body*> bodyPtrs;
	AITester() = default;
	~AITester() = default;
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};