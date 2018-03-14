#pragma once
#include "base/Scene.h"
#include "out/SpriteRenderer.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"

class AITester : public fk::Scene {
  public:
	fk::SpriteBatch sprites;
	fk::SpriteRenderer spriteRenderer;
	std::vector<int> spriteIDs;
	fk::Camera cam;
	glm::vec2 camMovement{ 0 };
	//b2World world;
	AITester() = default;
	~AITester() = default;
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};