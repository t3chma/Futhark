#pragma once
#include "base/Scene.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "out/TTFont.h"
#include "out/SpriteRenderer.h"

class GameWorld : public fk::Scene {
  public:
	int rightBind{ 0 };
	int rightInfo{ 0 };
	int leftBind{ 0 };
	int leftInfo{ 0 };
	fk::TTFont font;
	fk::SpriteBatch* textBatchPtr{ nullptr };
	fk::Sprites* spritesPtr{ nullptr };
	fk::Camera cam;
	fk::SpriteRenderer spriteRenderer;
	GameWorld() = default;
	~GameWorld() = default;
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};