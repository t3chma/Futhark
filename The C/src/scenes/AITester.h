#pragma once
#include "base/Scene.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "../actors/Player.h"
#include "../objects/Mouse.h"
#include "out/TTFont.h"
#include "out/WireRenderer.h"
#include "out/SpriteRenderer.h"

class AITester : public fk::Scene {
  public:
	fk::WireRenderer wireRenderer;
	int rightBind{ 0 };
	int rightInfo{ 0 };
	int leftBind{ 0 };
	int leftInfo{ 0 };
	Mouse* mousePtr{ nullptr };
	fk::TTFont font;
	fk::SpriteBatch* textBatchPtr{ nullptr };
	fk::SpriteRenderer spriteRenderer;
	fk::Camera cam;
	fk::World world;
	Player* playerPtr;
	std::vector<Actor*> actorPtrs;
	AITester() = default;
	~AITester() = default;
	void getCommandLine(fk::Tools& tools);
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};